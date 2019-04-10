/***************************************************************************************
*  Title: Topology Test Slave
* Authors: Jimmy Wong, Rowan Baker-French
* Date: February 10, 2019
* Code version: 0.0.3
***************************************************************************************/

#include <Wire_slave.h>
#include <T25Setup.h>
#include <T25Tile.h>
#include <T25SlaveTile.h>
#include <T25Common.h>

volatile bool addressUpdateFlag;

SlaveTile slave(I2C_DEFAULT);
HardwareTimer sensorTimer(3); // time for polling the sensors

void setup()
{
  afio_cfg_debug_ports(AFIO_DEBUG_NONE);
  addressUpdateFlag = false;

  //I2C Setup
  Wire.begin(I2C_DEFAULT);         // join i2c bus with the default address
  Wire.onRequest(requestEvent);    // register event
  Wire.onReceive(receiveAddress);  // register event

    ///////////////// SENSOR POLL TIMER SETUP ////////////////////
  sensorTimer.pause();

  sensorTimer.setPeriod(SENSOR_POLL_PERIOD); // in microseconds

  sensorTimer.setChannel1Mode(TIMER_OUTPUT_COMPARE);
  sensorTimer.setCompare(TIMER_CH1, 1);  // Interrupt 1 count after each update
  sensorTimer.attachCompare1Interrupt(sensorRead);

  sensorTimer.refresh();

  sensorTimer.resume();
  ///////////////////////////////////////////////////

  // Serial Setup - for output
  // Serial.begin(115200);    
  
  slave.beginSlaveTile();       
}

void loop()
{
  slave.readSensorData();
  
  if (addressUpdateFlag) {
    slave.debugWithMatrix(3, 3, GREEN);
    delay(125);
    //Turn off I2C and reinitialize
    struct TILE tileData = slave.getData();
    Wire.begin(tileData.addr); //join the i2c bus with a different address
    Wire.onRequest(requestEvent);
    Wire.onReceive(receiveEvent);
    addressUpdateFlag = false;   
  }

  // if(slave.getOperationMode() == MIRROR_MODE) {
  //   slave.debugWithMatrix(6, 6, BLUE);
  // }
  // if(slave.getOperationMode() == SCROLL_MODE) {
  //   slave.debugWithMatrix(4, 6, RED);
  // }
// DEFUNCT TOPOLOGY PINS
  // slave.findNeighborTiles();
// DEFUNCT TOPOLOGY PINS

  // make color from 2 bytes
  struct MessageData msgData = slave.getMessageData();

  if (msgData.color != slave.currentColor ) {
    slave.currentColor = msgData.color;
    // slave.changeColor(msgData.color);
  }
  if (msgData.brightness != slave.currentBrightness ) {
    slave.currentBrightness = msgData.brightness;
    // slave.setBrightness(slave.currentBrightness);
  }
  // UPDATE DISPLAY
  if (msgData.frame != slave.currentFrame ){
    slave.currentFrame = msgData.frame;
    slave.updateTileDisplay(msgData.pos, msgData.text);
  }
  delay(1);
}

/*
receiveEvent - function that executes whenever data is received from master
this function is registered as an event, see setup()
  Inputs: 
    void
  Outputs:
    dataLength - size of the i2c message
*/
void receiveEvent(int howMany)
{
  char mode = '0';

  if (Wire.available()) {
    mode = Wire.read();      // receive first byte as a character
  }
  switch(mode){
    case I2C_CHAR_KEY:
      // slave.debugWithMatrix(0, 0, RED);
      slave.receiveI2cData();
      slave.setOperationMode(SCROLL_MODE);
      break;
    case MIRROR_KEY:
      // slave.debugWithMatrix(3, 3, GREEN);
      slave.receiveI2cData();
      slave.setOperationMode(MIRROR_MODE);
      break;
    case AMBIENT_KEY:
      // slave.debugWithMatrix(6, 6, BLUE);
      slave.receiveI2cData();
      slave.setOperationMode(AMBIENT_MODE);
      break;
    default:
      // slave.debugWithMatrix(6, 0, YELLOW);
      slave.setOperationMode(MIRROR_MODE);
      break;
  }
}

// function that executes whenever data is requested by master
// this function is registered as an event, see setup()
void requestEvent()
{
  struct TILE data = slave.getData();
  Wire.write(data.ports);  // respond with message of 1 byte
}

void receiveAddress(int howMany){
  char c;
  if(Wire.available()){
    c = Wire.read();
  }
  if(c == 'A'){
    uint8_t addr = Wire.read();
    slave.setAddress(addr);
  }
  addressUpdateFlag = true;
}

void sensorRead() {
  slave.ISR_sensorRead();
}