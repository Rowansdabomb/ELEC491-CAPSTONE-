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
#include <Adafruit_GFX.h>
#include <Adafruit_DotStarMatrix.h>
#include <Adafruit_DotStar.h>

const uint8_t SUCCESS = 0;

const uint8_t MATRIX_FRAME_RATE = 4;
const uint8_t I2C_DEFAULT = 0x42;

const uint8_t TILE_MAX = 5;
const uint8_t DEBUG = 0;
const uint8_t ARRAY_SIZE  = 7;

//Use layout to store the addresses of the devices 7 by 7
int layout[ARRAY_SIZE][ARRAY_SIZE]= {
  {  0,  0,  0,  0,  0,  0,  0},
  {  0,  0,  0,  0,  0,  0,  0},
  {  0,  0,  0,  0,  0,  0,  0},
  {  0,  0,  0,  9,  0,  0,  0},
  {  0,  0,  0,  0,  0,  0,  0},
  {  0,  0,  0,  0,  0,  0,  0},
  {  0,  0,  0,  0,  0,  0,  0},
};

uint8_t tileOrder[4] = {0, 0, 0, 0};

TILE tile[TILE_MAX];

//last set as default 
int addr_lst[TILE_MAX] = {0x08, 0x10, 0x18, 0x20, 0x28};

//int error;
int tileID;
uint8_t x_free;
uint8_t y_free;
uint8_t tileCount;
bool directionChangedFlag;
bool tileReorderFlag;
//int tileCount_pre;

Adafruit_DotStarMatrix matrix = Adafruit_DotStarMatrix(
  matrixWidth, 
  matrixHeight, 
  tilesX, 
  tilesY,
  MATRIX_DATA_PIN, 
  MATRIX_CLK_PIN, 
  DS_MATRIX_TOP     + DS_MATRIX_LEFT +
  DS_MATRIX_COLUMNS + DS_MATRIX_ZIGZAG + DS_TILE_PROGRESSIVE,
  DOTSTAR_RGB
);

const uint16_t colors[] = {
  matrix.Color(255, 0, 0), matrix.Color(0, 255, 0), matrix.Color(0, 0, 255), matrix.Color(255, 255, 255) 
};

volatile bool i2cUpdateFlag = false;
HardwareTimer fpsTimer(2); // timer for updating the screen (send i2c data)

void setup() {
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

  tileReorderFlag = false;

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

uint8_t x = matrix.width();
uint8_t colorIndex = 0;
int col = 0;

void loop() {
  if(i2cUpdateFlag) {
    uint8_t array_x_max = 3;
    uint8_t array_y_max = 3;
    uint8_t array_x_min = 3;
    uint8_t array_y_min = 3;
    
    //Determine occupied directions
    getOccupiedDirections(tile);
  
    // Loop to check if the currently existing tiles
    // still exist, if not clear and erase from layout
    // tile[0] will be reserved for the master
    //tileCount_pre = tileCount;
    tileCount = 1;
  
    Wire.beginTransmission(I2C_DEFAULT); 
    if (!Wire.endTransmission()){
      Serial.println("Default Address still detected");
    }

    // 
    for(uint8_t i = 0; i < TILE_MAX; i++){
      uint8_t error = -1;
      
      //Check if dealing with master tile
      if( i != 0 ){
        if( tile[i].active ){
          Wire.beginTransmission(tile[i].addr);
          error = Wire.endTransmission();
        } else {
          tileID = i;
        }

        // Address successfully found
        if (error == SUCCESS) {
          adjustMapBounds(tile[i], array_x_min, array_x_max, array_y_min, array_y_max);
        } 
        // Address not found
        else {
          
          layout[tile[i].pos.y][tile[i].pos.x] = 0;
          tile[i].pos.x = 0;
          tile[i].pos.y = 0;
          tile[i].active = false;
          //Tile Removed 
          tileReorderFlag = true;
        }// END Address Successfully found
      }

      // New tile added
      if(tile[i].ports != tile[i].ports_pre){
        directionChangedFlag = true;
        uint8_t newDirection = tile[i].ports ^ tile[i].ports_pre;
        switch(newDirection){
         case CNCT_U:
            x_free = tile[i].pos.x;
            y_free = tile[i].pos.y - 1;
            break;
          case CNCT_D:
            x_free = tile[i].pos.x;
            y_free = tile[i].pos.y + 1;
            break;
          case CNCT_L:
            x_free = tile[i].pos.x - 1;
            y_free = tile[i].pos.y;
            break;
          case CNCT_R:
            x_free = tile[i].pos.x + 1;
            y_free = tile[i].pos.y;
            break;
          default:
            //will not happen
            break;
        }// END SWITCH
      }//End of Directional ports changing
    }// End FOR loop

    if(directionChangedFlag) {
      uint8_t result = assignNewAddress(tile, tileID, y_free, x_free));
      if (result = -1) {
        Serial.println("Failed to assign new address to tile with tileID: %d", tileID);
      }
      directionChangedFlag = false;// reset the direction changed flag
      tileReorderFlag = true;// raise the flag for to redo the tile order
    }

    // tile configuration has changed
    if(tileReorderFlag){ 
      configTileOrder(tileOrder, tile, array_y_min, array_y_max, array_x_min, array_x_max);
      tileReorderFlag = false;
    }
  
    for(uint8_t i = 0; i < tileCount; ++i){
      //tileOrder[i] is the index of the tile
      if(tileOrder[i] == 9){
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
      }else{
        struct POS temp;
        temp.x = col;
        temp.y = 0;
        transmitI2cData(tile[tileOrder[i]].addr, temp, colors[i], i);
      }
    }
  
    ++col;
    if (col > (array_x_max - array_x_min + 1)*matrixWidth) col = 0;
    i2cUpdateFlag = false;
  }
}

void adjustMapBound(TILE &tile, uint8_t &array_x_min, uint8_t &array_x_max, uint8_t array_y_min, uint8_t array_y_max) {
  tile.ports_pre = tile.ports;
  //If available request current port status from slave devices
  Wire.requestFrom(tile.addr, 1);
  tile.ports = Wire.read();
  tileCount++;
  if( tile.pos.x < array_x_min ){
    array_x_min = tile.pos.x;
  }
  if( tile.pos.x > array_x_max ){
    array_x_max = tile.pos.x;
  }
  if( tile.pos.y > array_y_max  ){
    array_y_max = tile.pos.y;
  }
  if( tile.pos.y < array_y_min ){
    array_y_min = tile.pos.y;
  }
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

uint8_t assignNewAddress(TILE *tile, const uint8_t &tileID, uint8_t &y_free, uint8_t &x_free) {
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
    tile[tileID].pos.x = x_free;   
    tile[tileID].pos.y = y_free;  
    layout[y_free][x_free] = tileID;
    x_free = 0;
    y_free = 0;
    return 0;
  }
    return -1;
}

void configTileOrder(uint8_t *tileOrder, TILE *tile, uint8_t &y_min, uint8_t &y_max, uint8_t &x_min, uint8_t &x_max) {
  // reset the order
  for(uint8_t i = 0; i < sizeof(tileOrder)/sizeof(uint8_t); ++i){
    tileOrder[i] = 0;
  }
  uint8_t cnt_order = 0;
  for(uint8_t y = y_min; y <= y_max; ++y){
    for(uint8_t x = x_min; x <= x_max; ++x){
      uint8_t currentTileID = layout[y][x];
      
      if (currentTileID == 9){
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

void transmitI2cData(const int &addr, const struct POS &pos, const uint16_t &color, uint8_t index) {
    Wire.beginTransmission(addr);
    Wire.write('B');
    Wire.write(pos.x);
    Wire.write(pos.y);
    Wire.write(color);
    Wire.write(index);
    Wire.endTransmission();
}
