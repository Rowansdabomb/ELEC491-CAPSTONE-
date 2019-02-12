/***************************************************************************************
*  Title: Topology Test Master
* Author: Jimmy Wong
* Date: February 10, 2019
* Code version: 0.0.3
***************************************************************************************/

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_DotStarMatrix.h>
#include <Adafruit_DotStar.h>


#define MATRIX_FRAME_RATE 2
#define I2C_DEFAULT 0x42

#define CNCT_U B0001
#define CNCT_D B0010
#define CNCT_L B0100
#define CNCT_R B1000

#define PIN_DIR_U PA4
#define PIN_DIR_D PA6
#define PIN_DIR_L PA3
#define PIN_DIR_R PA5

#define TILE_MAX 5

#define DEBUG 0

int print_flag = 0;
#define PRINT_EN 1

#define ARRAY_SIZE 7

//Use layout to store the addresses of the devices 7 by 7
int layout[7][7]= {{  0,  0,  0,  0,  0,  0,  0},
                  {  0,  0,  0,  0,  0,  0,  0},
                  {  0,  0,  0,  0,  0,  0,  0},
                  {  0,  0,  0,  9,  0,  0,  0},
                  {  0,  0,  0,  0,  0,  0,  0},
                  {  0,  0,  0,  0,  0,  0,  0},
                  {  0,  0,  0,  0,  0,  0,  0},
};

int tile_order[4] = {0, 0, 0, 0};

struct POS {
  uint8_t x;
  uint8_t y;
};

/* Tile structure
 * active = is this tile currently active
 * addr   = address of the current tile
 * posX   = position of tile in the X direction
 * posY   = position of tile in the Y direction
 * ports  = state of the directional pins
 * ports_pre = state of the directional pins in the previous instance 
*/
struct TILE {
  byte  active;
  int   addr;
  POS   pos;
  int   ports;
  int   ports_pre;
};



TILE tile[TILE_MAX];

//last set as default 
int addr_lst[TILE_MAX] = {0x08, 0x10, 0x18, 0x20, 0x28};

//int error;
int i;
int tileID;
int x_free;
int y_free;
int dirChange;
int dirChange_f;
int tile_count;
int tile_order_f = 0;
//int tile_count_pre;
int show_tile;
int show_x;
int show_y;

int pinDir = B0000;

//DotStar Setup
const uint8_t MATRIX_DATA_PIN = PB11; 
const uint8_t MATRIX_CLK_PIN = PB10;
const uint8_t CHAR_WIDTH = 5;
const uint8_t CHAR_HEIGHT = 8;

// Size of each tile matrix
const uint8_t matrixWidth = 4;
const uint8_t matrixHeight = 4;

// Number of tile matrices
const uint8_t tilesX = 1;
const uint8_t tilesY = 1;

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

void handler_tim(void);

void show_tile_info(int tileID);

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
  for(i = 1; i < TILE_MAX; ++i){
    tile[i].active  = 0;
    tile[i].addr    = addr_lst[i];
    tile[i].pos.x    = 0;
    tile[i].pos.y    = 0;
    tile[i].ports   = B00000000;
  }

  tile[0].active = 1;
  tile[0].addr = 0xFF;
  tile[0].pos.x = 3;
  tile[0].pos.y = 3;
  

//  //Timer for testing purposes
//  Timer2.setMode(TIMER_CH1, TIMER_OUTPUTCOMPARE);
//  Timer2.setPeriod(1000000);
//  Timer2.setCompare(TIMER_CH1, 1);
//  Timer2.attachInterrupt(TIMER_CH1, handler_tim);
  
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

  tile_order_f = 1;

    ////////////////// Timer setup ////////////////////
  fpsTimer.pause();

  fpsTimer.setPeriod(1000*1000/MATRIX_FRAME_RATE); // in microseconds

  fpsTimer.setChannel1Mode(TIMER_OUTPUT_COMPARE);
  fpsTimer.setCompare(TIMER_CH1, 1);  // Interrupt 1 count after each update
  fpsTimer.attachCompare1Interrupt(i2cUpdate);

  fpsTimer.refresh();

  fpsTimer.resume();
  ///////////////////////////////////////////////////
}



int x = matrix.width();
int pass = 0;
uint8_t colorIndex = 0;

int col = 0;

void loop() {
  if(i2cUpdateFlag) {
    Serial.println();
    
    int array_x_max = 3;
    int array_y_max = 3;
    int array_x_min = 3;
    int array_y_min = 3;
    
    //Serial.println("Determining Directions");
    
    //Determine occupied directions
    tile[0].ports_pre = tile[0].ports;
    tile[0].ports = B0000;
    if(digitalRead(PIN_DIR_U)){
      Serial.println("U");
      tile[0].ports = tile[0].ports | CNCT_U;
    }
    if(digitalRead(PIN_DIR_D)){
      Serial.println("D");
      tile[0].ports = tile[0].ports | CNCT_D;
    }
    if(digitalRead(PIN_DIR_L)){
      Serial.println("L");
      tile[0].ports = tile[0].ports | CNCT_L;
    }
    if(digitalRead(PIN_DIR_R)){
      Serial.println("R");
      Serial.println(tile[0].ports, BIN);
      tile[0].ports = tile[0].ports | CNCT_R;
      Serial.println(tile[0].ports, BIN);
      Serial.println(tile[0].ports_pre, BIN);
    }
  
    // Loop to check if the currently existing tiles
    // still exist, if not clear and erase from layout
    // tile[0] will be reserved for the master
    //tile_count_pre = tile_count;
    tile_count = 1;
  
    //Serial.println("First I2C Check");
    Wire.beginTransmission(I2C_DEFAULT); 
    if (!Wire.endTransmission()){
      Serial.println("Default Address still detected");
    }
    
    for(i = 0; i < TILE_MAX; i++){
      int error = -1;
      //Serial.print("Currently Checking Tile ");
      //Serial.println(i);
      if( i != 0 ){//Check if dealing with master tile
        if( tile[i].active == 1 ){
          Wire.beginTransmission(tile[i].addr);
          error = Wire.endTransmission();
        }else{
          tileID = i;
        }
        
        if (error == SUCCESS) {
          if(DEBUG){
            Serial.print("I2C device found at address 0x");
            Serial.println(tile[i].addr, HEX);
          }//END DEBUG PRINT
          tile[i].ports_pre = tile[i].ports;
          //If available request current port status from slave devices
          Wire.requestFrom(tile[i].addr, 1);
          tile[i].ports = Wire.read();
          tile_count++;
          if( tile[i].pos.x < array_x_min ){
            array_x_min = tile[i].pos.x;
          }
          if( tile[i].pos.x > array_x_max ){
            array_x_max = tile[i].pos.x;
          }
          if( tile[i].pos.y > array_y_max  ){
            array_y_max = tile[i].pos.y;
          }
          if( tile[i].pos.y < array_y_min ){
            array_y_min = tile[i].pos.y;
          }
        }else{
          if(DEBUG){
            Serial.print("No I2C device found at address 0x");
            Serial.println(tile[i].addr, HEX);
          }// END DEBUG PRINT
          layout[tile[i].pos.y][tile[i].pos.x] = 0;
          tile[i].pos.x = 0;
          tile[i].pos.y = 0;
          tile[i].active = 0;
          //Tile Removed 
          tile_order_f = 1;
        }// END Address Successfully found
   
      }
  
      //Check if the directional ports has changed
//      Serial.println(tile[i].ports);
//      Serial.println(tile[i].ports_pre);
      if(tile[i].ports != tile[i].ports_pre){
        dirChange_f = 1;
        dirChange = tile[i].ports ^ tile[i].ports_pre;
        switch(dirChange){
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

    Serial.println(dirChange_f);
    if(dirChange_f == 1 ){
      
      // Check if the default address exist
      Wire.beginTransmission(I2C_DEFAULT);
      int def_error = Wire.endTransmission();
      Serial.println(def_error);
      if (def_error == SUCCESS){
        Serial.println("Device found at default address");
        Wire.beginTransmission(I2C_DEFAULT);
        Wire.write('A');
        Wire.write(tile[tileID].addr); //Assign the next available address from 
        Wire.endTransmission();  
        //Maybe insert something here during the connection process?
        Serial.print("Sent address: ");
        Serial.println(tile[tileID].addr, HEX);
      }
  
      delay(500); //Half second delay before checking that the tile is now in place
      //Determine the location of the tile
      Wire.beginTransmission(tile[tileID].addr);
      int addr_error = Wire.endTransmission();
      if (addr_error == 0){
        tile[tileID].active = 1;
        tile[tileID].pos.x = x_free;   
        tile[tileID].pos.y = y_free;  
        layout[y_free][x_free] = tileID;
        x_free = 0;
        y_free = 0;
        dirChange_f = 0;// reset the direction changed flag
        tile_order_f = 1;// raise the flag for to redo the tile order
      }
  
    }
    if(print_flag == PRINT_EN){
      Serial.println("Current Internal Array");
      for(int j = 0; j < ARRAY_SIZE; j++){
        for(int k = 0; k < ARRAY_SIZE; k++){
          Serial.print(layout[j][k]);
          Serial.print(" ");
        }
        Serial.println();
      }
      print_flag = 0;
    }
    if(print_flag == 5){
      Serial.print("Array X Values: ");
      Serial.print(array_x_min);
      Serial.print(" ");
      Serial.println(array_x_max);
      Serial.print("Array Y Values: ");
      Serial.print(array_y_min);
      Serial.print(" ");
      Serial.println(array_y_max);
      print_flag = 0;
    }
    
  
    //TODO: Sending data dynamically
    if(tile_order_f == 1){ // only needs to be done if number of tiles changes
      int cnt_x;
      int cnt_y;
      int cnt_order = 0;
      for(cnt_order = 0; cnt_order < 4; cnt_order++){
            tile_order[cnt_order] = 0;  //reset the order
      }
      cnt_order = 0;
      for(cnt_y = array_y_min; cnt_y <= array_y_max; cnt_y++){
        for(cnt_x = array_x_min; cnt_x <= array_x_max; cnt_x++){
          /*Serial.print("At position ");
          Serial.print(cnt_x);
          Serial.print(" ");
          Serial.println(cnt_y);
          */
          int temp_id = layout[cnt_y][cnt_x];
          if (temp_id == 9){
            tile_order[cnt_order] = temp_id;
            cnt_order++;
          }else if (temp_id != 0){
            if( tile[temp_id].active == 1){
              tile_order[cnt_order] = temp_id;
              cnt_order++;
            }       
          }
        }
        tile_order_f = 0;
      }// End looping through array
    }// END if 
    
    //tile_order[x] is the index of the tile
  
    for(uint8_t i = 0; i < tile_count; ++i){
      
      if(tile_order[i] == 9){
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
        transmitI2cData(tile[tile_order[i]].addr, temp, colors[i], i);
      }
    }
  
//  delay(100);
    ++col;
    if (col > (array_x_max - array_x_min)*matrixWidth) col = 0;
    i2cUpdateFlag = false;
  }
  
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

void handler_tim(void) {
  print_flag = 1;
}

void show_tile_info(int tileID){
  Serial.print("Tile ID: ");
  Serial.print(tileID);
  Serial.print(" x: ");
  Serial.print(tile[tileID].pos.x);
  Serial.print(",y: ");
  Serial.print(tile[tileID].pos.y);
  Serial.print(",active: ");
  Serial.print(tile[tileID].active);
  Serial.print(", ports: ");
  Serial.println(tile[tileID].ports, BIN);
}
