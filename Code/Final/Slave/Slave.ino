/***************************************************************************************
*  Title: Topology Test Slave
* Authors: Jimmy Wong, Rowan Baker-French
* Date: February 10, 2019
* Code version: 0.0.3
***************************************************************************************/

#include <Wire_slave.h>
#include <T25Tile.h>
#include <T25SlaveTile.h>

volatile bool addressUpdateFlag;

SlaveTile slave(I2C_DEFAULT);

uint16_t prevColor = 0;

void setup()
{
  addressUpdateFlag = false;

  //I2C Setup
  Wire.begin(I2C_DEFAULT);         // join i2c bus with the default address
  Wire.onRequest(requestEvent);    // register event
  Wire.onReceive(receiveAddress);  // register event

  //  Serial Setup - for output
  Serial.begin(57600);    
  
  slave.beginSlaveTile();       
}

void loop()
{
  if (addressUpdateFlag == 1){
    //Turn off I2C and reinitialize
    struct TILE tileData = slave.getData();
    Wire.begin(tileData.addr); //join the i2c bus with a different address
    Wire.onRequest(requestEvent);
    Wire.onReceive(receiveEvent);
    Serial.print("Attempted Address Change");
    Serial.println(tileData.addr, HEX); 
    addressUpdateFlag = 0;   
  }
  slave.findNeighborTiles();

  // make color from 2 bytes
  struct MessageData msgData = slave.getMessageData();

  if (msgData.color != prevColor ) {
    prevColor = msgData.color;
    Serial.print("Color from master: ");
    Serial.println(msgData.color);
    slave.changeColor(msgData.color);
  }

  Serial.print("pos x: ");
  Serial.print(msgData.pos.x);
  Serial.println();

  slave.updateTileDisplay(msgData.pos, msgData.text);

  delay(10);
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
    case 'B':
      slave.receiveI2cData();
      slave.setOperationMode(DIRECTION_TEST);
      break;
    case 'Q':
      slave.receiveI2cData();
      slave.setOperationMode(SCROLL_MODE);
      break;
    default:
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
  addressUpdateFlag = 1;
}