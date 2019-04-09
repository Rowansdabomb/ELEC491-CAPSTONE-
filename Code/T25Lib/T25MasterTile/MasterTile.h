#ifndef t25_master_h
#define t25_master_h

#include <Arduino.h>
#include <T25Tile.h>
#include <T25Setup.h>
#include <T25Common.h>

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
    // INITIALIZATION
    MasterTile(uint8_t addr);
    void beginMasterTile();

    // SETTERS
    void setTileCount(const uint8_t count);
    void setTextData(const char text[], const uint8_t size);

    // GETTERS
    uint8_t getTileCount();
    uint8_t getOrderedTileID(uint8_t i);
    struct TILE getTile(uint8_t i);
    struct POS getScrollPos();

    void resetTileOrder();
    void resetTileMapBounds();
    void updateScrollPos();
    uint8_t handleDisplayShape();
    void updateFromDataBase();

    void transmitToSlave(const uint8_t addr, const struct POS &pos, char data[], const uint8_t frame);
    struct POS getOutputData(char dataOut[], const uint8_t tileIndex);
    char textData[MAX_STRING_SIZE];

    struct POS preConfiguredDisplay[3];

  private:
    uint16_t textDataLength;
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
    uint8_t transmitI2cCharData(const uint8_t addr, const struct POS &pos, char data[]);
    uint8_t transmitMirrorData(const uint8_t addr);
    uint8_t transmitAmbientData(const uint8_t addr, const uint8_t frame);
    uint8_t transmitGestureData(const uint8_t addr);
    void adjustMapBounds(struct TILE &tile);
    void configTileOrder();
};

#endif
