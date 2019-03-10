#include "MasterTile.h"

MasterTile::MasterTile(uint8_t addr):Tile(addr) {
  // Initialize the tileMap
  for(uint8_t i = 0; i < TILE_MAP_SIZE; i++) {
    for(uint8_t j = 0; j < TILE_MAP_SIZE; j++) {
      tileMap[i][j] = 0;
      // set center of map as MASTER_TILE_ID
      if(i = TILE_MAP_SIZE/2 && j == TILE_MAP_SIZE/2) tileMap[i][j] = MASTER_TILE_ID;
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

  // configure data
  data.active = true;
  data.addr = 0xFF;
  data.pos.x = 3;
  data.pos.y = 3;
}

/*
transmitI2cCharData - Transmits the Character Data to a slave tile.
  Inputs:
    addr    - address of the slave tile
    pos     - reference position of where character data should start
    color   - RGB value to display text
    data    - 2 byte character array
  Outputs:
    Return value of endTransmission
*/
uint8_t assignNewAddress(const uint8_t yFree, const uint8_t xFree) {
  // Check if the default address exist
  uint8_t response = -1;
  Wire.beginTransmission(I2C_DEFAULT);
  response = Wire.endTransmission();
  if (response != SUCCESS) return response;
  
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
  tile[tileID].pos.x = xFree;   
  tile[tileID].pos.y = yFree;  
  tileMap[yFree][xFree] = tileID;
  return SUCCESS;
}

/*
transmitI2cCharData - Transmits the Character Data to a slave tile.
  Inputs:
    addr    - address of the slave tile
    pos     - reference position of where character data should start
    color   - RGB value to display text
    data    - 2 byte character array
  Outputs:
    Return value of endTransmission
*/
int transmitI2cCharData(const int &addr, const struct POS &pos, const uint16_t &color, char data[]) {
    Wire.beginTransmission(addr);
    Wire.write('Q'); // New Identifier for sending Character data? using Q arbritrarily 
    Wire.write(pos.x);
    Wire.write(pos.y);
    Wire.write(color);
    for(int i = 0; i < MAX_DISPLAY_CHARS; ++i){ //For 4x4 should be 2
        Wire.write(data[i]);
    }
    return Wire.endTransmission();
}

/*
getOutputData - gets the characters that are to be sent to a Tile
  Inputs:
    dataOut - Character Array with MAX_DISPLAY_CHARS size 
    textData - Text Array with textLength size
    textLength - The length of the text to be displayed as obtained from Sanket's code
  Outputs:
    N/A
*/
void getOutputData(char dataOut[], char textData[], const uint8_t textLength){
  for( int i = 0; i < MAX_DISPLAY_CHARS; ++i){
    if ( i == 0 ){
      dataOut[i] = textData[startIndex];
    }else{
      if( startIndex + i + 1 > textLength){
        dataOut[i] = ' '; //Data out of bounds set to empty character
      }else{
        dataOut[i] = textData[startIndex + i];
      }
    }
  }
}
