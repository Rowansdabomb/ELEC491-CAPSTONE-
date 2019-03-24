#ifndef t25_slave_h
#define t25_slave_h

#include "Arduino.h"
#include <T25Tile.h>
#include <T25Setup.h>

class SlaveTile: public Tile {
public:
  SlaveTile(uint8_t addr);
  void beginSlaveTile();

  uint8_t receiveI2cData();
  void setAddress(const uint8_t addr);
  struct MessageData getMessageData();
//   static void receiveEvent(int howMany);
//   static void requestEvent();

private:
  char msgBuffer[8];

//   static void receiveAddress(int howMany);
};

#endif