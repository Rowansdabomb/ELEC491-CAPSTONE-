#include "Arduino.h"
#include "Tile.h"
#include "MatrixSetup.h"
#include <Wire.h>

Tile::Tile(uint8_t addr) {
  data.addr = addr;
  cursor.x = 0;
  cursor.y = 0;
  cursorStart.x = cursor.x/CHAR_WIDTH;
  cursorStart.y = cursor.y/CHAR_HEIGHT;
}



void Tile::updateTileDisplay(const int i) {
    matrix.fillScreen(0);
    if((data.ports & CNCT_U) == CNCT_U){
      matrix.fillRect(1, 3, 2, 1, colors[i]);
    }
    if((data.ports & CNCT_D) == CNCT_D){
      matrix.fillRect(1, 0, 2, 1, colors[i]);
    }
    if((data.ports & CNCT_L) == CNCT_L){
      matrix.fillRect(0, 1, 1, 2, colors[i]);
    }
    if((data.ports & CNCT_R) == CNCT_R){
      matrix.fillRect(3, 1, 1, 2, colors[i]);
    }
    matrix.fillRect(1, 1, 2, 2, colors[VIOLET]);
    matrix.show();
}

// void getOccupiedDirections() {  
void Tile::findNeighborTiles() {  
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
}

/*
displayChar - Shows the visible portion of characters on the matrix
  Inputs:
    dataOut - char array of characters to be displayed
*/

void Tile::displayChar(char dataOut[]) {
  matrix.fillScreen(0);
  matrix.setCursor(cursor.x, cursor.y);
  for(int i = 0; i < MAX_DISPLAY_CHARS; ++i){ //For 4x4 should be 2
    matrix.print(dataOut[i]);
  }
  matrix.show();
}