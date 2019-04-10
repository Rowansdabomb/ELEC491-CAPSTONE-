#include "SlaveTile.h"
#include "Constants.h"
#include "PinConfig.h"
#include "Colors.h"
#include <Wire_slave.h>
#include <T25Common.h>

SlaveTile::SlaveTile(uint8_t addr):Tile(addr) {
  for (uint8_t i = 0; i < 8; ++i) {
    msgBuffer[i] = 0;
  }
}

void SlaveTile::beginSlaveTile() {
    beginTile();
}

/*
receiveI2cData - 
  Inputs:
    void
  Outputs:
    msgSize - size of message received in bytes
*/
uint8_t SlaveTile::receiveI2cData() {
  uint8_t msgSize = 0;
  // Serial.println("Recieve I2C data: ");
  while (Wire.available()) {
    msgBuffer[msgSize] = static_cast<uint8_t> (Wire.read());
    // Serial.print(msgBuffer[msgSize]);
    msgSize++;
    if (msgSize > 254) break;
  }
  // Serial.println();
  return msgSize;
}

/*
setAddress - 
  Inputs:
    addr - the address to be set
  Outputs:

*/
void SlaveTile::setAddress(const uint8_t addr) {
  data.addr = addr;
}

/*
getMessageData - // Serializes message data depending on operation mode
    Inputs:
        void
    Outputs:
        msgData -   a struct containing the cursor, color, and data to be 
                    displayed on the matrix
*/
struct MessageData SlaveTile::getMessageData() {
    struct MessageData msgData;

    msgData.color = (msgBuffer[0] << 8 ) | (msgBuffer[1] & 0xff);
    msgData.brightness = msgBuffer[2];
    msgData.frame = msgBuffer[3];

    switch(operationMode) {
        case SCROLL_MODE:
          msgData.pos.x = msgBuffer[4];
          msgData.pos.y = msgBuffer[5];
          
          for(uint8_t i = 0; i < MAX_DISPLAY_CHARS; i++) {
              msgData.text[i] = msgBuffer[6 + i];
          }
          break;
        case MIRROR_MODE:
        case AMBIENT_MODE:
        case GESTURE_MODE:
          break;
        default:
          break;
    }

    return msgData;
}

