#include "SlaveTile.h"
#include "Constants.h"
#include "PinConfig.h"
#include "Colors.h"
#include <Wire_slave.h>

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
  while (Wire.available()) {
    msgBuffer[msgSize] = static_cast<uint8_t> (Wire.read());
    msgSize++;
    if (msgSize > 254) break;
  }
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
getMessageData - serializes message data depending on operation mode
    Inputs:
        void
    Outputs:
        msgData -   a struct containing the cursor, color, and data to be 
                    displayed on the matrix
*/
struct MessageData SlaveTile::getMessageData() {
    struct MessageData msgData;
    switch(operationMode) {
        case SCROLL_MODE:
            msgData.pos.x = msgBuffer[0];
            msgData.pos.y = msgBuffer[1];
            msgData.color = (msgBuffer[2] << 8 ) | (msgBuffer[3] & 0xff);
            for(uint8_t i = 0; i < MAX_DISPLAY_CHARS; i++) {
                msgData.text[i] = msgBuffer[3 + i];
            }
            break;
        case GESTURE_MODE:
            break;
        default:
            break;
    }

    return msgData;
}

