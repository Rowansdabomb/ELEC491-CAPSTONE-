#ifndef t25_tile_h
#define t25_tile_h

#include "Arduino.h"

struct POS {
  int8_t x;
  int8_t y;
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
    void setCursor(int8_t x, int8_t y);
    struct TILE getData();
    struct TILE findNeighborTiles();
    void debugWithMatrix(const uint8_t x, const uint8_t y, const uint8_t color);
    void updateOperationMode(const uint8_t mode);
    void updateTileDisplay(const uint8_t i, char dataOut[]);

  protected:
    uint8_t operationMode;
    struct TILE data;
    struct POS cursor;
    struct POS cursorStart;

    void displayChar(char dataOut[]);
};

#endif
