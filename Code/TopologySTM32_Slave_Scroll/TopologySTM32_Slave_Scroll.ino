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

volatile bool addressUpdateFlag;
volatile bool matrixUpdateFlag;
volatile bool matrixScrollFlag;
// I2C 
uint8_t I2C_ADDR = 0x42; //Initalize I2C_ADDR
#define I2C_DEFAULT 0x42

uint8_t posX = 0;
uint8_t posY = 0;
uint8_t ports = B0000;
uint8_t dataLength = 0;

char msgBuffer[8] = {0, 0, 0, 0, 0, 0, 0, 0};

void setup()
{
  addressUpdateFlag = false;
  matrixUpdateFlag = false;
  matrixScrollFlag = false;

  //I2C Setup
  Wire.begin(I2C_DEFAULT);         // join i2c bus with the default address
  Wire.onRequest(requestEvent); // register event
  Wire.onReceive(receiveAddress); // register event
  
  // Directional Pin Setup
  pinMode(PIN_DIR_U, INPUT_PULLDOWN);
  pinMode(PIN_DIR_D, INPUT_PULLDOWN);
  pinMode(PIN_DIR_L, INPUT_PULLDOWN);
  pinMode(PIN_DIR_R, INPUT_PULLDOWN);

  // DotStar Setup - BEGIN 
  matrix.begin();
  matrix.setBrightness(64); // Set max brightness 
  matrix.setTextWrap(false);
  matrix.setTextColor(colors[0]);
  matrix.show();  // Turn all LEDs off ASAP

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
  ports = getOccupiedDirections();

  //make color from 2 bytes
  posX = msgBuffer[0];
  posY = msgBuffer[1];
  uint16_t color = (msgBuffer[2] << 8 ) | (msgBuffer[3] & 0xff);
  dataLength = dataLength - 4;
  

  if(matrixUpdateFlag){
    updateTileDisplay(msgBuffer[4]);
    matrixUpdateFlag = 0;
  }
  if(matrixScrollFlag){
    matrixScroll(posX, posY, 4, dataLength);
    matrixScrollFlag =0;
  }

  delay(100);
}

uint8_t receiveI2cData(volatile bool &enableFlag,  char msgBuffer[]){
  enableFlag = 1;
  uint8_t msgCount = 0;
  while(Wire.available()){
    msgBuffer[msgCount] = (uint8_t) Wire.read();
    Serial.print(msgCount);
    Serial.print(" ");
    Serial.println(msgBuffer[msgCount], BIN);
    msgCount++;

  }
  return msgCount;
}

uint8_t getOccupiedDirections(){
  uint8_t tempPorts = B0000;
  if(digitalRead(PIN_DIR_U)){
    tempPorts = tempPorts | CNCT_U;
  }
  if(digitalRead(PIN_DIR_D)){
    tempPorts = tempPorts | CNCT_D;
  }
  if(digitalRead(PIN_DIR_L)){
    tempPorts = tempPorts | CNCT_L;
  }
  if(digitalRead(PIN_DIR_R)){
    tempPorts = tempPorts | CNCT_R;
  }
  return tempPorts;
}

// function that executes whenever data is received from master
// this function is registered as an event, see setup()
void receiveEvent(int howMany)
{
  char c;
  Serial.println("Event Received");
  if(Wire.available()){
    c = Wire.read();      // receive first byte as a character
    Serial.println(c);         // print the character
  }
  if(c == 'B'){
    dataLength = receiveI2cData(matrixUpdateFlag, msgBuffer);

  }
  if(c == 'Q'){
    dataLength = receiveI2cData(matrixScrollFlag, msgBuffer);
    
  }
}

void updateTileDisplay(const int i) {
    matrix.fillScreen(0);
    if((ports & CNCT_U) == CNCT_U){
      matrix.fillRect(1, 3, 2, 1, colors[i]);
    }
    if((ports & CNCT_D) == CNCT_D){
      matrix.fillRect(1, 0, 2, 1, colors[i]);
    }
    if((ports & CNCT_L) == CNCT_L){
      matrix.fillRect(0, 1, 1, 2, colors[i]);
    }
    if((ports & CNCT_R) == CNCT_R){
      matrix.fillRect(3, 1, 1, 2, colors[i]);
    }
    matrix.fillRect(1, 1, 2, 2, colors[3]);
    matrix.show();
}

void matrixScroll(uint8_t &posX, uint8_t &posY, uint8_t dataStart, uint8_t &dataLength){
  matrix.fillScreen(0);
  matrix.setCursor(posX, posY);
  for(uint8_t i = dataStart; i < dataLength; ++i){
    matrix.print(msgBuffer[i]);
  }
  matrix.show();
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

