#ifndef t25_master_h
#define t25_master_h

#include "Constants.h"
#include "Tile.h"

// library interface description
class MasterTile: public Tile
{
  // user-accessible "public" interface
  public:
    MasterTile(uint8_t addr);
    uint8_t assignNewAddress( const uint8_t yFree, const uint8_t xFree); 
    uint8_t transmitI2cCharData(const int &addr, const uint16_t &color, char data[]);

  private:
    uint8_t cursor;
    uint8_t tileID;
    uint8_t tileMap[TILE_MAP_SIZE][TILE_MAP_SIZE];
    struct TILE tile[TILE_MAX];

    void updateScrollPos(uint16_t &scrollPos, const uint8_t scrollLength)
    void getOutputData(char dataOut[], char textData[], const uint8_t textLength);
    //Currently only a prototype for when Sanket implements his code as well
    void updateTextData(uint8_t &scrollLength);

};

#endif
