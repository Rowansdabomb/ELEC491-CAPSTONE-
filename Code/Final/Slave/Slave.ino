/***************************************************************************************
*  Title: Topology Test Slave
* Author: Jimmy Wong
* Date: February 10, 2019
* Code version: 0.0.3
***************************************************************************************/

#include <Wire_slave.h>
#include "PinConfig.h"
#include "MatrixSetup.h"
#include "Colors.h"
#include "Constants.h"

volatile bool addressUpdateFlag;
volatile bool matrixUpdateFlag;
volatile bool matrixScrollFlag;

uint8_t posX = 0;
uint8_t posY = 0;
uint8_t ports = B0000;
uint8_t dataLength = 0;

// char msgBuffer[8] = {0, 0, 0, 0, 0, 0, 0, 0};

SlaveTile slave(I2C_DEFAULT);

void setup()
{
  addressUpdateFlag = false;
  matrixUpdateFlag = false;
  matrixScrollFlag = false;

  //  Serial Setup - for output
  Serial.begin(9600);           
}

void loop()
{
//  Serial.println("loop");
  if (addressUpdateFlag == 1){
    //Turn off I2C and reinitialize
    Wire.begin(I2C_ADDR); //join the i2c bus with a different address
    Wire.onRequest(requestEvent);
    Wire.onReceive(receiveEvent);
    Serial.print("Attempted Address Change");
    Serial.println(I2C_ADDR, HEX); 
    addressUpdateFlag = 0;   
  }
  slave.findNeighborTiles();

  // make color from 2 bytes
  struct POS pos;
  cursor.x = msgBuffer[0];
  cursor.y = msgBuffer[1];
  uint16_t color = (msgBuffer[2] << 8 ) | (msgBuffer[3] & 0xff);
  dataLength = dataLength - 4;

  struct displayData data;
  data.text = msgBuffer[4];
  updateTileDisplay(data);

  delay(100);
}


// function that executes whenever data is requested by master
// this function is registered as an event, see setup()
void requestEvent()
{
  Wire.write(ports);  // respond with message of 1 byte
                      // as expected by master
}

void receiveAddress(int howMany){
  char c;
  if(Wire.available()){
    c = Wire.read();
  }
  if(c == 'A'){
    I2C_ADDR = Wire.read();
    Serial.println(I2C_ADDR, HEX);
  }
  addressUpdateFlag = 1;
}

