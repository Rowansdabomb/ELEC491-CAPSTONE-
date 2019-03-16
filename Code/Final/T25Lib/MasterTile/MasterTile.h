#ifndef t25_master_h
#define t25_master_h

#include "Arduino.h"
#include "../Tile/Tile.h"
#include "../Constants.h"

struct TILEMAP {
  uint8_t grid[TILE_MAP_SIZE][TILE_MAP_SIZE];
  uint8_t xMin;
  uint8_t xMax;
  uint8_t yMin;
  uint8_t yMax;
};

class MasterTile: public Tile
{
  public:
    MasterTile(uint8_t addr);

    void beginMasterTile();

    // Setters
    void setTileCount(const uint8_t count);
    void setTextData(const char textData[], const uint16_t size);

    // Getters
    uint8_t getTileCount();
    uint8_t getOrderedTileID(uint8_t i);
    struct TILE getTile(uint8_t i);
    struct POS getOffsetCursor(uint8_t tileNumber, uint8_t charXIndex);
    struct POS getScrollPos();

    void resetTileOrder();
    void resetTileMapBounds();
    void updateScrollPos();
    uint8_t handleDisplayShape();

    void transmitToSlave(const uint8_t addr, const struct POS &pos, const uint16_t color, char data[]);
    uint8_t getOutputData(char dataOut[], char textData[], const uint8_t textLength, const uint8_t charIndex);


  private:
    char textData[MAX_STRING_SIZE];
    uint16_t scrollLength;
    struct POS scrollPos;

    uint8_t tileID;
    uint8_t tileCount;
    uint8_t tileOrder[TILE_MAX];
    struct TILEMAP tileMap;
    struct TILE tile[TILE_MAX];

    void handleAddedOrRemovedTiles();
    void addNewTile(const struct TILE &tile);
    uint8_t assignNewAddress(const uint8_t yFree, const uint8_t xFree); 
    void addressNotFound(struct TILE &tile);
    uint8_t transmitI2cCharData(const uint8_t addr, const struct POS &pos, const uint16_t color, char data[]);
    void adjustMapBounds(struct TILE &tile);
    void configTileOrder();
};

#endif
