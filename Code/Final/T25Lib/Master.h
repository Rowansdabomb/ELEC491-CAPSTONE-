/*
  Master.h - Test library for Wiring - description
  Copyright (c) 2006 John Doe.  All right reserved.
*/

// ensure this library description is only included once
#ifndef t25_master_h
#define t25_master_h

#include <stdint.h>
#include "Constants.h"
#include "Tile.h"

// library interface description
class MasterTile: public Tile
{
  // user-accessible "public" interface
  public:
    MasterTile(uint8_t addr);
    int transmitI2cCharData(const int addr, const struct POS pos, const uint16_t color, const char data[]);

  // library-accessible "private" interface
  private:
    struct TILE tile[TILE_MAX];
    struct TILE data;
    void doSomethingSecret(void);
};

#endif
