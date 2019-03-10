#ifndef t25_tile_h
#define t25_tile_h
#include <stdint.h>
#include "PinConfig.h"
#include "Colors.h"

struct POS {
  uint8_t x;
  uint8_t y;
};

/* Tile structure
 * active = is this tile currently active
 * addr   = address of the current tile
 * posX   = position of tile in the X direction
 * posY   = position of tile in the Y direction
 * ports  = state of the directional pins
 * previousPorts = state of the directional pins in the previous instance 
*/

struct TILE {
  bool  active;
  int   addr;
  POS   pos;
  int   ports;
  int   previousPorts;
};

class Tile {
  public:
    Tile(uint8_t addr);
  protected:
    struct TILE data;

    struct POS getOffset(uint8_t scrollPos, uint8_t tileNumber);
    void updateTileDisplay(const int i);
    void updateScrollPos(uint8_t &scrollPos, const uint8_t scrollLength);
    void displayChar(const POS &cursor, char dataOut[]);
    void findNeighborTiles();
}

#endif
