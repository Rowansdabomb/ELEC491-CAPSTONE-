#include "Arduino.h"
#include "Constants.h"
#include "Tile.h"
#include "PinConfig.h"
#include "Colors.h"
#include <Wire.h>

Tile::Tile(uint8_t addr) {
  data.addr = addr;
  cursor.x = 0;
  cursor.y = 0;
  cursorStart.x = cursor.x/CHAR_WIDTH;
  cursorStart.y = cursor.y/CHAR_HEIGHT;

  //initialize the matrix
  matrix = new Adafruit_DotStarMatrix(
    MATRIX_WIDTH, 
    MATRIX_HEIGHT, 
    TILES_X, 
    TILES_Y,
    MATRIX_DATA_PIN, 
    MATRIX_CLK_PIN, 
    DS_MATRIX_TOP     + DS_MATRIX_LEFT +
    DS_MATRIX_COLUMNS + DS_MATRIX_ZIGZAG + DS_TILE_PROGRESSIVE,
    DOTSTAR_RGB
  );

  // DotStar Setup
  matrix->begin(); // Initialize pins for output
  matrix->setBrightness(64); // Set max brightness (out of 255) 
  matrix->setTextWrap(false);
  matrix->setTextColor(colors[0]);
  matrix->show();  // Turn all LEDs off ASAP
}

/*
setCursor - sets the cursor relative to the top left of the tile matrix
  Inputs:
    x - the horizontal offset from the left edge of the tile matrix
    y - the veritcal offset from the top edge of the tile matrix
*/
void Tile::setCursor(int8_t x, int8_t y) {
  cursor.x = x;
  cursor.y = y;
}

/*
getData - retrieves the Tile's status, address, position, and findNeighborTiles
  Outputs:
    TILE - a struct describing the tile data
*/
struct TILE Tile::getData() {
  return data;
}

/*
updateTileDisplay - updates the display based on current operation mode
*/
void Tile::updateTileDisplay(const uint8_t i, char dataOut[]) {
    switch(operationMode) {
      case (SCROLL_MODE):
        displayChar(dataOut);
        break;
      case (GESTURE_MODE):
        // TBD
        break;
      case (DIRECTION_TEST):
        // i2cDirectionTest(i);
        break;
    }
}

/*
displayChar - Shows the visible portion of characters on the matrix
  Inputs:
    dataOut - char array of characters to be displayed
*/
void Tile::displayChar(char dataOut[]) {
  matrix->fillScreen(0);
  matrix->setCursor(cursor.x, cursor.y);
  for(int i = 0; i < MAX_DISPLAY_CHARS; ++i){ //For 4x4 should be 2
    matrix->print(dataOut[i]);
  }
  matrix->show();
}

// /*
// i2cDirectionTest - test to show direction of tiles added
// */
// void Tile::i2cDirectionTest(const uint8_t i) {
//     matrix->fillScreen(0);
//     if((data.ports & CNCT_U) == CNCT_U){
//       matrix->fillRect(1, 3, 2, 1, colors[i]);
//     }
//     if((data.ports & CNCT_D) == CNCT_D){
//       matrix->fillRect(1, 0, 2, 1, colors[i]);
//     }
//     if((data.ports & CNCT_L) == CNCT_L){
//       matrix->fillRect(0, 1, 1, 2, colors[i]);
//     }
//     if((data.ports & CNCT_R) == CNCT_R){
//       matrix->fillRect(3, 1, 1, 2, colors[i]);
//     }
//     matrix->fillRect(1, 1, 2, 2, colors[WHITE]);
//     matrix->show();
// }

/*
findNeighborTiles - checks each port of the tile for a neighbor and updates the Tile's data
  Outputs:
    struct TILE - data including updated ports of the current tile
*/
// void getOccupiedDirections() {  
struct TILE Tile::findNeighborTiles() {  
  // remember previous ports
  data.previousPorts = data.ports;

  // get current ports
  data.ports = 0b0000;
  if(digitalRead(PIN_DIR_U)){
    data.ports = data.ports | CNCT_U;
  }
  if(digitalRead(PIN_DIR_D)){
    data.ports = data.ports | CNCT_D;
  }
  if(digitalRead(PIN_DIR_L)){
    data.ports = data.ports | CNCT_L;
  }
  if(digitalRead(PIN_DIR_R)){
    data.ports = data.ports | CNCT_R;
  }

  return data;
}

/*
debugWithMatrix - Displays single pixel on tile matrix corresponding to some error code
  Inputs:
    x - horizontal offset from left edge of tile matrix
    y - veritcal offset from top edge of tile matrix
    color - predefined color code value
*/
void Tile::debugWithMatrix(const uint8_t x, const uint8_t y, const uint8_t color) {
  matrix->fillScreen(0);
  matrix->fillRect(x, 1, y, 1, colors[color]);
  matrix->show();
  delay(250);
}