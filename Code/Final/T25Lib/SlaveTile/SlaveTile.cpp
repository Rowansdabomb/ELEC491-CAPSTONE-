#include "SlaveTile.h"

SlaveTile::SlaveTile(uint8_t addr):Tile(addr) {
  for (uint9_t i = 0; i < 8; ++i) {
    msgBuffer[i] = 0;
  }

  //I2C Setup
  Wire.begin(data.addr);         // join i2c bus with the default address
  Wire.onRequest(requestEvent); // register event
  Wire.onReceive(receiveAddress); // register event
}

/*
receiveI2cData - 
  Inputs:
    &enableFlag - 
  Outputs:
    msgSize - size of message received in bytes
*/
uint8_t SlaveTile::receiveI2cData(const char msgBuffer[]) {
  uint8_t msgSize;
  while (Wire.available()) {
    msgBuffer[i] = static_cast<uint8_t> Wire.read();
    msgSize++;
  }
  return msgSize;
}

/*
setAddress - 
  Inputs:
    addr - the address to be set
  Outputs:
    msgSize - size of message received in bytes
*/
uint8_t SlaveTile::setAddress(const uint8_t addr) {
  data.addr = adrr;
  return msgSize;
}

/*
receiveEvent - function that executes whenever data is received from master
this function is registered as an event, see setup()
  Inputs: 
    void
  Outputs:
    dataLength - size of the i2c message
*/
uint8_t SlaveTile::void receiveEvent()
{
  uint8_t dataLength = 0;
  char mode = '0';

  Serial.println("Event Received");
  if (Wire.available()) {
    mode = Wire.read();      // receive first byte as a character
    Serial.println(mode);         // print the character
  }
  switch(mode){
    case 'B':
      dataLength = receiveI2cData(msgBuffer);
      setOperationMode(DIRECTION_TEST);
      break;
    case 'Q':
      dataLength = receiveI2cData(msgBuffer);
      setOperationMode(SCROLL_MODE);
      break;
    default:
      break;
  }

  return dataLength
}

/*
matrixScroll - prints to the matrix each
  Inputs:
    &pos - location to set cursor on matrix
    data - c string of data to be output to matrix
    dataLength - length of data
  Outputs:
    void
*/
void SlaveTile::matrixScroll(struct POS &pos, char data[], const uint8_t dataLength) {
  matrix->fillScreen(0);
  matrix->setCursor(pos.x, pos.y);
  for(uint8_t i = 0; i < dataLength, ++i){
    matrix->print(data[i]);
  }
  matrix->show();
}

/*
requestEvent - function that executes whenever data is requested by master
this function is registered as an event, see setup()
  Inputs:
    void
  Outputs:
    void
*/
void SlaveTile::requestEvent() {
  Wire.write(data.ports);  // respond with message of 1 byte
                      // as expected by master
}

/*
receiveAddress - 
  Inputs:
    howMany - 
  Outputs:
    void
*/
void SlaveTile::receiveAddress(int howMany) {
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