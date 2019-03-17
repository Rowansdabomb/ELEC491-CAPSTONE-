#ifndef t25_tile_h
#define t25_tile_h

#include "Arduino.h"
#include "Constants.h"
#include <Adafruit_GFX.h>
#include <Adafruit_DotStarMatrix.h>
#include <Adafruit_DotStar.h>

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
  bool      active;
  uint8_t   addr;
  POS       pos;
  uint8_t   ports;
  uint8_t   previousPorts;
};

struct MessageData {
    struct POS pos;
    uint16_t color;
    char text[MAX_DISPLAY_CHARS];
};

class Tile {
  public:
    Tile(uint8_t addr);

    char msgBuffer[8] = {0, 0, 0, 0, 0, 0, 0, 0};

    uint8_t frameRate;

    void beginTile();

    void setCursor(int8_t x, int8_t y);
    void setOperationMode(const uint8_t mode);
    uint8_t getOperationMode();
    struct TILE getData();

    struct TILE findNeighborTiles();
    void debugWithMatrix(const uint8_t x, const uint8_t y, const uint8_t color);
   
    void updateTileDisplay(const POS &outPos, char dataOut[]);

    Adafruit_DotStarMatrix *matrix;

  protected:
    uint8_t operationMode;
    struct TILE data;
    struct POS cursor;

    void displayChar(const POS &pos, char dataOut[]);
    void i2cDirectionTest(const uint16_t color);
};

#endif
