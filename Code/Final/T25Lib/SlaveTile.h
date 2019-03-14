#ifndef t25_slave_h
#define t25_slave_h

#include "Tile.h"

class SlaveTile: public Tile {
public:
  SlaveTile(uint8_t addr);
  uint8_t receiveI2cData(const char msgBuffer[]);
  uint8_t setAddress(uint8_t addr);

private:
  char msgBuffer[8];
  
  void receiveEvent(int howMany);
  void matrixScroll(struct POS &pos, char data[], const uint8_t dataLength);
  void requestEvent();
  void receiveAddress(int howMany);
}

#endif