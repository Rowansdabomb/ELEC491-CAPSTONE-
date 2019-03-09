/***************************************************************************************
*  Title: Topology Test Master
* Author: Jimmy Wong
* Date: February 10, 2019
* Code version: 0.0.3
***************************************************************************************/

#include <Wire.h>
#include "Tile.h"
#include "PinConfig.h"
#include "MatrixSetup.h"
#include "Constants.h"
#include "Colors.h"

uint8_t tileOrder[4] = {0, 0, 0, 0};

TILE tile[TILE_MAX];

//Use tileMap to store the addresses of the devices 7 by 7
int tileMap[ARRAY_SIZE][ARRAY_SIZE]= {
  {  0,  0,  0,         0,        0,  0,  0},
  {  0,  0,  0,         0,        0,  0,  0},
  {  0,  0,  0,         0,        0,  0,  0},
  {  0,  0,  0,  MASTER_TILE_ID,  0,  0,  0},
  {  0,  0,  0,         0,        0,  0,  0},
  {  0,  0,  0,         0,        0,  0,  0},
  {  0,  0,  0,         0,        0,  0,  0},
};

//last set as default 
const uint8_t addr_lst[TILE_MAX] = {0x08, 0x10, 0x18, 0x20, 0x28};

int tileID;
bool tileReorderFlag;

volatile bool i2cUpdateFlag;
HardwareTimer fpsTimer(2); // timer for updating the screen (send i2c data)

void setup() {
  // Flag initialization
  tileReorderFlag = false;
  i2cUpdateFlag = false;
  
  // Directional Pin Setup
  pinMode(PIN_DIR_U, INPUT_PULLDOWN);
  pinMode(PIN_DIR_D, INPUT_PULLDOWN);
  pinMode(PIN_DIR_L, INPUT_PULLDOWN);
  pinMode(PIN_DIR_R, INPUT_PULLDOWN);
  pinMode(PA2, OUTPUT);
  
  // Internal Device Map - Initial Population
  for(uint8_t i = 1; i < TILE_MAX; ++i){
    tile[i].active  = false;
    tile[i].addr    = addr_lst[i];
    tile[i].pos.x    = 0;
    tile[i].pos.y    = 0;
    tile[i].ports   = B00000000;
  }
  
  // Master Tile Setup
  tile[0].active = true;
  tile[0].addr = 0xFF;
  tile[0].pos.x = 3;
  tile[0].pos.y = 3;
  
  // I2C Master Setup
  Wire.begin();

  // DotStar Setup
  matrix.begin(); // Initialize pins for output
  matrix.setBrightness(64); // Set max brightness (out of 255) 
  matrix.setTextWrap(false);
  matrix.setTextColor(colors[0]);
  matrix.show();  // Turn all LEDs off ASAP

  // Serial Setup - for output
  Serial.begin(9600); 

  /////////////////// Timer setup ////////////////////
  fpsTimer.pause();

  fpsTimer.setPeriod(1000*1000/MATRIX_FRAME_RATE); // in microseconds

  fpsTimer.setChannel1Mode(TIMER_OUTPUT_COMPARE);
  fpsTimer.setCompare(TIMER_CH1, 1);  // Interrupt 1 count after each update
  fpsTimer.attachCompare1Interrupt(i2cUpdate);

  fpsTimer.refresh();

  fpsTimer.resume();
  ///////////////////////////////////////////////////
}

void loop() {
  if(i2cUpdateFlag) {
    uint8_t tileCount = 0;
    tileCount = handleDisplayShape();  
    for(uint8_t i = 0; i < tileCount; ++i){
      //tileOrder[i] is the index of the tile
      if (tileOrder[i] == MASTER_TILE_ID) {
        updateTileDisplay(i);
      } else {
        transmitToSlaves(i);
      }
    }
    i2cUpdateFlag = false;
  }
}

void transmitToSlaves(const int i) {
    struct POS temp;
    temp.x = 0;
    temp.y = 0;
    transmitI2cData(tile[tileOrder[i]].addr, temp, colors[i]);
}

void updateTileDisplay(const int i) {
    matrix.fillScreen(0);
    if((tile[0].ports & CNCT_U) == CNCT_U){
      matrix.fillRect(1, 3, 2, 1, colors[i]);
    }
    if((tile[0].ports & CNCT_D) == CNCT_D){
      matrix.fillRect(1, 0, 2, 1, colors[i]);
    }
    if((tile[0].ports & CNCT_L) == CNCT_L){
      matrix.fillRect(0, 1, 1, 2, colors[i]);
    }
    if((tile[0].ports & CNCT_R) == CNCT_R){
      matrix.fillRect(3, 1, 1, 2, colors[i]);
    }
    matrix.fillRect(1, 1, 2, 2, colors[3]);
    matrix.show();
}

uint8_t handleDisplayShape() {
    uint8_t array_x_max = 3;
    uint8_t array_y_max = 3;
    uint8_t array_x_min = 3;
    uint8_t array_y_min = 3;
    
    //Determine occupied directions
    getOccupiedDirections(tile);
  
    // Loop to check if the currently existing tiles
    // still exist, if not clear and erase from tileMap
    // tile[0] will be reserved for the master
    uint8_t tileCount = 1;

    for(uint8_t i = 0; i < TILE_MAX; i++){
      //Check if dealing with master tile
      if( i != 0 ){
        uint8_t response = 4;
        if( tile[i].active ){
          Wire.beginTransmission(tile[i].addr);
          response = Wire.endTransmission();
        } else {
          tileID = i;
        }

        // Address successfully found
        switch(response) {
          case 0:
            ++tileCount;
            adjustMapBounds(tile[i], array_x_min, array_x_max, array_y_min, array_y_max);
            break;
          case 1:
            Serial.println("I2C ERROR: DATA TOO LONG");
            addressNotFound(tile[i], tileReorderFlag);
            break;
          case 2:
            Serial.println("I2C ERROR: NACK ON TRANSMIT OF ADDRESS");
            addressNotFound(tile[i], tileReorderFlag);
            break;
          case 3: 
            Serial.println("I2C ERROR: NACK ON TRANSMIT OF DATA");
            addressNotFound(tile[i], tileReorderFlag);
            break;
          case 4:
            Serial.println("I2C ERROR: OTHER ERROR RESPONSE FROM I2C");
            addressNotFound(tile[i], tileReorderFlag);
            break;
          default:
            Serial.println("I2C ERROR: UNKNOWN ERROR RESPONSE FROM I2C");
            addressNotFound(tile[i], tileReorderFlag);
        } // END SWITCH
      } // END NOT MASTER IF

      // New tile added
      if(tile[i].ports != tile[i].ports_pre){
        uint8_t newDirection = tile[i].ports ^ tile[i].ports_pre;
        uint8_t xFree;
        uint8_t yFree;
        switch(newDirection){
         case CNCT_U:
            xFree = tile[i].pos.x;
            yFree = tile[i].pos.y - 1;
            break;
          case CNCT_D:
            xFree = tile[i].pos.x;
            yFree = tile[i].pos.y + 1;
            break;
          case CNCT_L:
            xFree = tile[i].pos.x - 1;
            yFree = tile[i].pos.y;
            break;
          case CNCT_R:
            xFree = tile[i].pos.x + 1;
            yFree = tile[i].pos.y;
            break;
          default:
            //will not happen
            break;
        }// END SWITCH

        assignNewAddress(tileID, yFree, xFree);
        tileReorderFlag = true;// raise the flag for to redo the tile order
      }//End of Directional ports changing
    }// End FOR loop

    // tile configuration has changed
    if(tileReorderFlag){ 
      configTileOrder(array_y_min, array_y_max, array_x_min, array_x_max);
      tileReorderFlag = false;
    }

    return tileCount;
}

void debugWithMatrix(int , int color) {
  matrix.fillScreen(0);
  matrix.fillRect(0, 0, 0, 0, colors[color]);
}

void adjustMapBounds(TILE &tile, uint8_t &xMin, uint8_t &xMax, uint8_t &yMin, uint8_t &yMax) {
  tile.ports_pre = tile.ports;
  //If available request current port status from slave devices
  Wire.requestFrom(tile.addr, 1);
  tile.ports = Wire.read();
  if( tile.pos.x < xMin ){
    xMin = tile.pos.x;
  }
  if( tile.pos.x > xMax ){
    xMax = tile.pos.x;
  }
  if( tile.pos.y > yMax  ){
    yMax = tile.pos.y;
  }
  if( tile.pos.y < yMin ){
    yMin = tile.pos.y;
  }
}

void addressNotFound(TILE &tile, bool &tileReorderFlag) {
    tileMap[tile.pos.y][tile.pos.x] = 0;
    tile.pos.x = 0;
    tile.pos.y = 0;
    tile.active = false;
    //Tile Removed 
    tileReorderFlag = true;
}

void getOccupiedDirections(TILE *tile) {  
  // remember previous ports
  tile[0].ports_pre = tile[0].ports;

  // get current ports
  tile[0].ports = B0000;
  if(digitalRead(PIN_DIR_U)){
    tile[0].ports = tile[0].ports | CNCT_U;
  }
  if(digitalRead(PIN_DIR_D)){
    tile[0].ports = tile[0].ports | CNCT_D;
  }
  if(digitalRead(PIN_DIR_L)){
    tile[0].ports = tile[0].ports | CNCT_L;
  }
  if(digitalRead(PIN_DIR_R)){
    tile[0].ports = tile[0].ports | CNCT_R;
  }
}

uint8_t assignNewAddress(const uint8_t &tileID, uint8_t &yFree, uint8_t &xFree) {
  // Check if the default address exist
  Wire.beginTransmission(I2C_DEFAULT);
  if (Wire.endTransmission() == SUCCESS){
    Wire.beginTransmission(I2C_DEFAULT);
    Wire.write('A');
    Wire.write(tile[tileID].addr); //Assign the next available address from 
    Wire.endTransmission();  
  }

  delay(500); //Half second delay before checking that the tile is now in place
  //Determine the location of the tile
  Wire.beginTransmission(tile[tileID].addr);
  if (Wire.endTransmission() == SUCCESS){
    tile[tileID].active = true;
    tile[tileID].pos.x = xFree;   
    tile[tileID].pos.y = yFree;  
    tileMap[yFree][xFree] = tileID;
    return 0;
  }
  return -1;
}

void configTileOrder(const uint8_t &yMin, const uint8_t &yMax, const uint8_t &xMin, const uint8_t &xMax) {
  // reset the order
  for(uint8_t i = 0; i < sizeof(tileOrder)/sizeof(uint8_t); ++i){
    tileOrder[i] = 0;
  }

  uint8_t cnt_order = 0;
  for(uint8_t y = yMin; y <= yMax; ++y){
    for(uint8_t x = xMin; x <= xMax; ++x){
      uint8_t currentTileID = tileMap[y][x];
      
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

void i2cUpdate() {
  i2cUpdateFlag = true;
}

void transmitI2cData(const int &addr, const struct POS &pos, const uint16_t &color) {
    Wire.beginTransmission(addr);
    Wire.write('B');
    Wire.write(pos.x);
    Wire.write(pos.y);
    Wire.write(color);
    Wire.endTransmission();
}
