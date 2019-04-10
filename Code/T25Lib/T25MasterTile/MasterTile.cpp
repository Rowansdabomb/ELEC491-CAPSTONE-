#include "Arduino.h"
#include "MasterTile.h"
#include "PinConfig.h"
#include "Constants.h"
#include "MatrixSetup.h"
#include "Colors.h"
#include "CommonConstants.h"
#include <Wire.h>

MasterTile::MasterTile(uint8_t addr):Tile(addr) {
  tileID = 0;
  tileCount = 0;

  scrollPos.x = 0;
  scrollPos.y = 0;

}

void MasterTile::beginMasterTile() {
  // first initialize the base class Tile 
  beginTile();

  //Initialize the tileOrder
  resetTileOrder();

  // Initialize the tileMap
  tileMap.xMin = TILE_MAP_SIZE/2;
  tileMap.xMax = TILE_MAP_SIZE/2;
  tileMap.yMin = TILE_MAP_SIZE/2;
  tileMap.yMax = TILE_MAP_SIZE/2;
  for(uint8_t i = 0; i < TILE_MAP_SIZE; i++) {
    for(uint8_t j = 0; j < TILE_MAP_SIZE; j++) {
      tileMap.grid[i][j] = 0;
      // set center of map as MASTER_TILE_ID
      if ((i == TILE_MAP_SIZE/2) && (j == TILE_MAP_SIZE/2)) tileMap.grid[i][j] = MASTER_TILE_ID;
    }
  }

  // Internal Device Map - Initial Population
  for(uint8_t i = 1; i < TILE_MAX; ++i){
    tile[i].active  = false;
    tile[i].addr    = addr_lst[i];
    tile[i].pos.x   = 0;
    tile[i].pos.y   = 0;
    tile[i].ports   = 0b00000000;
  }

  // Master Tile Setup
  data.active = true;
  data.pos.x = 3;
  data.pos.y = 3;

  tile[0] = data;

  setTextData(" Hello", 6);

  // I2C Master Setup
  Wire.begin();

  // PRECONFIGURED DISPLAY SETUP
  for (uint8_t i = 0; i < 3; ++i) {
    preConfiguredDisplay[i].x = 4 + i;
    preConfiguredDisplay[i].y = 3;
  }
}

void MasterTile::setTileCount(const uint8_t count) {
  tileCount = count;
}

uint8_t MasterTile::getTileCount() {
  return tileCount;
}

uint8_t MasterTile::getOrderedTileID(const uint8_t index) {
  return tileOrder[index];
}

struct POS MasterTile::getScrollPos() {
  return scrollPos;
}
/*
getTile - returns a tile
  Inputs:
    id - the id of the tile to be returned
  Outputs:
    tile - 
*/
struct TILE MasterTile::getTile(const uint8_t id) {
  return tile[id];
}

/*
resetTileOrder - sets the tileOrder array to all 0s
  Inputs:
    void
  Outputs:
    void
*/
void MasterTile::resetTileOrder() {
  for(uint8_t i = 0; i < TILE_MAX; ++i) {
    tileOrder[i] = 0;
  }
}

void MasterTile::resetTileMapBounds() {
  tileMap.xMax = TILE_MAP_SIZE/2;
  tileMap.xMin = TILE_MAP_SIZE/2;
  tileMap.yMax = TILE_MAP_SIZE/2;
  tileMap.yMin = TILE_MAP_SIZE/2;
}

/*
handleDisplayShape - checks for the addition or removal of Tiles from the display and
  updates the
  Inputs:
    void
  Outputs:
    tileCount - the current number of tiles in the display
*/
uint8_t MasterTile::handleDisplayShape() {
    resetTileOrder();
    resetTileMapBounds();

    //Determine occupied directions
// DEFUNCT TOPOLOGICAL PINS
    // findNeighborTiles();
// DEFUNCT TOPOLOGICAL PINS
    tile[0] = getData();
    handleAddedOrRemovedTiles();

    return tileCount;
}

/*
handleAddedOrRemovedTiles - Determines if any tiles have been added or removed and updates the tile order.
  Inputs:
    void
  Outputs:
    void
*/
void MasterTile::handleAddedOrRemovedTiles () {
    tileCount = 1;
    uint8_t tilesUpdated = false;

    // Loop to check if the currently existing tiles
    // still exist, if not clear and erase from tileMap
    // tile[0] will be reserved for the master
    for (uint8_t i = 0; i < TILE_MAX; i++) {
      //Deal with slave Tiles
      if ( i != 0 ) {
        uint8_t response = 5;
        if ( tile[i].active ) {
          Wire.beginTransmission(tile[i].addr);
          response = Wire.endTransmission();
        } else {
          tileID = i;
        }

        switch(response) {
          case 0:
          // SUCCESS
            ++tileCount;
            adjustMapBounds(tile[i]);
            break;
          case 1:
          // DATA TOO LONG
            debugWithMatrix(6, 0, VIOLET);
            addressNotFound(tile[i]);
            tilesUpdated = true; // Tile removed
            break;
          case 2:
          // NACK ON TRANSMIT OF ADDRESS
            debugWithMatrix(6, 2, ORANGE);
            addressNotFound(tile[i]);
            tilesUpdated = true; // Tile removed
            break;
          case 3:
          // NACK ON TRANSMIT OF DATA
            debugWithMatrix(6, 4, BLUE);
            addressNotFound(tile[i]);
            tilesUpdated = true; // Tile removed
            break;
          case 4:
          // OTHER ERROR
            debugWithMatrix(6, 6, GREEN);
            addressNotFound(tile[i]);
            tilesUpdated = true; // Tile removed
            break;
          default:
          // TILE NOT ACTIVE
            addressNotFound(tile[i]);
            tilesUpdated = true; // Tile removed
        } // END SWITCH
      } // END NOT MASTER IF

// DEFUNCT TOPOLOGICAL PINS
      // Check for new tiles
      // if (tile[i].ports != tile[i].previousPorts) {
      //   addNewTile(tile[i]);
      //   tilesUpdated = true; // Tile added
      // }
      uint8_t response = assignNewAddress(preConfiguredDisplay[tileCount - 1].y, preConfiguredDisplay[tileCount - 1].x);
      // if (response != SUCCESS) {
      //   debugWithMatrix(3, 3, YELLOW)
      // }
// DEFUNCT TOPOLOGICAL PINS

    }// End FOR loop

    if(tilesUpdated){
      configTileOrder();
    }
}

/*
addNewTile - finds the location for the new tile in the tile map
  Inputs:
    &tile - the new tile to be added
*/
void MasterTile::addNewTile(const struct TILE &tile) {
    uint8_t newDirection = tile.ports ^ tile.previousPorts;
    uint8_t x, y;
    switch(newDirection){
      case CNCT_U:
        x = tile.pos.x;
        y = tile.pos.y - 1;
        break;
      case CNCT_D:
        x = tile.pos.x;
        y = tile.pos.y + 1;
        break;
      case CNCT_L:
        x = tile.pos.x - 1;
        y = tile.pos.y;
        break;
      case CNCT_R:
        x = tile.pos.x + 1;
        y = tile.pos.y;
        break;
      default:
        x = 0;
        y = 0;
    }// END SWITCH

    uint8_t response = assignNewAddress(y, x);
}

/*
assignNewAddress - assigns an address to a newly connected Tile
  Inputs:
    y - y index of new tile in tileMap
    x - x index of new tile in tileMap
  Outputs:
    Return value of endTransmission
*/
uint8_t MasterTile::assignNewAddress(const uint8_t x, const uint8_t y) {
  // Check if the default address exist

  uint8_t response = -1;
  Wire.beginTransmission(I2C_DEFAULT);
  response = Wire.endTransmission();
  if (response != SUCCESS) {
    return response;
  } 

  Wire.beginTransmission(I2C_DEFAULT);
  Wire.write('A');
  Wire.write(tile[tileID].addr); //Assign the next available address from
  response = Wire.endTransmission();
  if (response != SUCCESS) return response;

  uint8_t waits = 0;
  response = -1;
  while(response != SUCCESS && waits < 20) {
    delay(10);
    Wire.beginTransmission(tile[tileID].addr);
    response = Wire.endTransmission();
    ++waits;
  }

  if (response != SUCCESS) return response;

  tile[tileID].active = true;
  tile[tileID].pos.x = x;
  tile[tileID].pos.y = y;
  tileMap.grid[y][x] = tileID;
  return SUCCESS;
}

/*
getOutputData - gets the characters that are to be sent to a Tile
  Inputs:
    dataOut     - Character Array with MAX_DISPLAY_CHARS size
    charIndex   - The index of the character the scroll position is currently at

        scrollPos = 2 and charIndex = 0
           V
    0  0  0| 0  0  .  0  .      .  .  .  .  .  .  .  .
    0  .  0| .  0  .  0  .      .  .  .  .  .  .  .  .
    .  .  0| .  .  .  0  .      .  .  .  .  .  .  .  .
    .  .  0| .  .  .  0  .      .  .  .  .  .  .  .  .
    .  .  0| .  .  .  0  .      .  .  .  .  .  .  .  . => TL
    .  .  0| .  .  .  0  .      .  .  .  .  .  .  .  .
    .  0  0| 0  .  .  0  0      0  0  0  .  .  .  .  .
    .  .  .| .  .  .  .  .      .  .  .  .  .  .  .  .

  Outputs:
    N/A
*/
struct POS MasterTile::getOutputData(char dataOut[], uint8_t tileIndex){
  uint8_t charIndex = ( scrollPos.x + MATRIX_WIDTH * tileIndex ) / CHAR_WIDTH;
  struct POS offset;
  offset.x = charIndex*CHAR_WIDTH - (scrollPos.x + MATRIX_WIDTH * tileIndex);
  offset.y = 0;
  for( int i = 0; i < MAX_DISPLAY_CHARS; ++i){ //TODO: Write function to determine MAX_DISPLAY_CHARS
    // if (charIndex ==)
    if (charIndex + i >= textDataLength) {
      // dataOut[i] = textData[0];
      dataOut[i] = ' ';
      // offset.x = MATRIX_WIDTH;
    } else {
      dataOut[i] = textData[charIndex + i];
    }
  }
  return offset;
}

/*
updateScrollPos - gets the current position in the scrolling string
*/
void MasterTile::updateScrollPos() {
  if (scrollPos.x >= scrollLength) {
    scrollPos.x = 0;
  } else {
    ++scrollPos.x;
  }
}

/*
adjustMapBounds - sets the outer bounds of the tileMap
  Inputs:
    &tile - tile struct
*/
void MasterTile::adjustMapBounds(struct TILE &tile) {
  tile.previousPorts = tile.ports;
  //If available request current port status from slave devices

// DEFUNCT TOPOLOGICAL PINS
  // Wire.requestFrom(tile.addr, 1);
  // tile.ports = Wire.read();
// DEFUNCT TOPOLOGICAL PINS
  if (tile.pos.x < tileMap.xMin){
    tileMap.xMin = tile.pos.x;
  }
  if (tile.pos.x > tileMap.xMax){
    tileMap.xMax = tile.pos.x;
  }
  if (tile.pos.y > tileMap.yMax){
    tileMap.yMax = tile.pos.y;
  }
  if (tile.pos.y < tileMap.yMin) {
    tileMap.yMin = tile.pos.y;
  }
}

/*
configTileOrder - sorts tileOrder with tilIDs from left to right then top to bottom

  tileMap.grid    =>    tileOrder
    0 0 0 0 0
    0 0 2 1 0
    0 0 9 3 0    =>    [2, 1, 9, 3]
    0 0 0 0 0
    0 0 0 0 0
*/
void MasterTile::configTileOrder() {
  resetTileOrder();

  uint8_t cnt_order = 0;
  for(uint8_t y = tileMap.yMin; y <= tileMap.yMax; ++y){
    for(uint8_t x = tileMap.xMin; x <= tileMap.xMax; ++x){
      uint8_t currentTileID = tileMap.grid[y][x];

      if (currentTileID == MASTER_TILE_ID){
        tileOrder[cnt_order] = currentTileID;
        ++cnt_order;
      }
      else if (currentTileID != 0 && tile[currentTileID].active){
        tileOrder[cnt_order] = currentTileID;
        ++cnt_order;
      }
    }
  }// End looping through array
}

/*
addressNotFound - resets the tilemap position and data of the missing tile
  Inputs:
    &tile - the tile that is not found
*/
void MasterTile::addressNotFound(struct TILE &tile) {
    tileMap.grid[tile.pos.y][tile.pos.x] = 0;
    tile.pos.x = 0;
    tile.pos.y = 0;
    tile.active = false;
}

/*
transmitToSlave - transmits data to a slave tile depending on operation mode
  Inputs:
    addr  - address of the slave tile to update
    pos   - cursor offset for slave tile
    data  - char array of data to be sent to slave
*/
void MasterTile::transmitToSlave(const uint8_t addr, const struct POS &pos, char data[]) {
    Wire.beginTransmission(addr);
    switch(operationMode) {
      case (SCROLL_MODE):
        writeI2cCharData(pos, data);
        break;
      case (AMBIENT_MODE):
        writeAmbientData();
        break;
      case (MIRROR_MODE):
        writeMirrorData();
        break;
      case (GESTURE_MODE):
        // TBD
        break;
    }
    Wire.endTransmission();
}

/*
writeI2cCharData - Transmits the Character Data to a slave tile.
  Inputs:
    addr    - address of the slave tile
    pos     - reference position of where character data should start
    data    - 2 byte character array
  Outputs:
    void
*/
void MasterTile::writeI2cCharData(const struct POS &pos, char data[]) {
    Wire.write(I2C_CHAR_KEY); // New Identifier for sending Character data? using Q arbritrarily
    
    writeMetaData();
    Wire.write(pos.x);
    Wire.write(pos.y);

    for(int i = 0; i < MAX_DISPLAY_CHARS; ++i){ //For 4x4 should be 2
        Wire.write(data[i]);
    }
}
/*
writeMirrorData - Transmits the Data required for Mirror mode to a slave tile 
  Inputs:
    addr    - address of the slave tile
    brightness - intensity of the LEDs 
    color   - RGB value of color to display in Mirror mode

  Outputs:
    void
*/
void MasterTile::writeMirrorData(){
  Wire.write(MIRROR_KEY);
  writeMetaData();
}

/*
writeAmbientData - Transmits the Data required for Ambient mode to a slave tile 
  Inputs:
    addr    - address of the slave tile
  Outputs:
    void 
*/
void MasterTile::writeAmbientData(){
  Wire.write(AMBIENT_KEY);
  writeMetaData();
}

/*
writeMetaData - a helper function to ensure meta data is always transmitted as
  1. Color
  2. brightness
  3. frame
*/
void MasterTile::writeMetaData() {
  Wire.write((currentColor >> 8) & 0xff);
  Wire.write(currentColor & 0xff);
  Wire.write(currentBrightness);
  Wire.write(currentFrame);
}

/*
setTextData - Sets scrollLength, textData, and textDataSize
  Inputs:
    text[] - c string containing the data to update textData with
    size - the length of the text string
  Outputs:
    void
*/
void MasterTile::setTextData(const char text[], uint8_t size) {
  strncpy (textData, text, size);
  textData[size] = '\0';
  textDataLength = size;
  scrollLength = size * CHAR_WIDTH;
}

/*
updateFromDataBase - 
  Inputs: void
  Outputs: void
*/
void MasterTile::updateFromDataBase() {
  uint8_t metaData[2] = {255}; // [0] is transmitType, [1] is size of data to be transmitted
  char msgBuf[MAX_STRING_SIZE] = {'\0'};

  Wire.requestFrom(WIFI_SLAVE_ADDR, 255);

  if(Wire.available()){
    metaData[0] = Wire.read();
    metaData[1] = Wire.read();
  }

  for(int msgCount = 0; msgCount < metaData[1]; ++msgCount){
    if (Wire.available()){
      msgBuf[msgCount] = Wire.read();
    }
  }
  
  while(Wire.available()){
    Wire.read();
  }
  switch(metaData[0]) {
    case CHANGE_COLOR:
    {
      currentColor = makeColor(msgBuf[0], msgBuf[1], msgBuf[2]);
      changeColor(currentColor);
      break;
    }
    case CHANGE_TEXT:
    {
      setTextData(msgBuf, metaData[1]); //This should be done by Sanket's code
      break;
    }
    case CHANGE_OPERATION_MODE:
    {
      setOperationMode(msgBuf[0]);
      break;
    }
    case CHANGE_BRIGHTNESS:
    {
      setBrightness(msgBuf[0]);
      break;
    }
    case CHANGE_SCROLL_SPEED:
    {
      setTargetFrameRate(msgBuf[0]);
      break;
    }
    default:
      // DO NOTHING
      break;
  }
}
