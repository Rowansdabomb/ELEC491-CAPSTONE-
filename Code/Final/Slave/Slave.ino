/***************************************************************************************
*  Title: Topology Test Slave
* Authors: Jimmy Wong, Rowan Baker-French
* Date: February 10, 2019
* Code version: 0.0.3
***************************************************************************************/

#include <Wire_slave.h>
#include <T25Lib.h>

volatile bool addressUpdateFlag;

SlaveTile slave(I2C_DEFAULT);

void setup()
{
  addressUpdateFlag = false;

  //I2C Setup
  Wire.begin(I2C_DEFAULT);         // join i2c bus with the default address
  Wire.onRequest(requestEvent); // register event
  Wire.onReceive(receiveAddress); // register event

  //  Serial Setup - for output
  Serial.begin(9600);    
  
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
  Serial.print(msgData.pos.x);
  Serial.print(" ");
  Serial.println(msgData.pos.y);

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

  Serial.println("Event Received");
  if (Wire.available()) {
    mode = Wire.read();      // receive first byte as a character
    Serial.println(mode);         // print the character
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
  Serial.println("Attempte receiveAddress");
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