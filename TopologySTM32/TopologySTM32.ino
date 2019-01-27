#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_DotStarMatrix.h>
#include <Adafruit_DotStar.h>

#define I2C_DEFAULT 0x42

#define CNCT_U B0001
#define CNCT_D B0010
#define CNCT_L B0100
#define CNCT_R B1000

#define PIN_DIR_U PA15
#define PIN_DIR_D PB3
#define PIN_DIR_L PB4
#define PIN_DIR_R PB5

#define TILE_MAX 5

//Use layout to store the addresses of the devices 7 by 7
int layout[7][7]= {{  0,  0,  0,  0,  0,  0,  0},
                  {  0,  0,  0,  0,  0,  0,  0},
                  {  0,  0,  0,  0,  0,  0,  0},
                  {  0,  0,  0,  1,  0,  0,  0},
                  {  0,  0,  0,  0,  0,  0,  0},
                  {  0,  0,  0,  0,  0,  0,  0},
                  {  0,  0,  0,  0,  0,  0,  0},
};

struct POS {
  int x;
  int y;
};

/* Tile structure
 * active = is this tile currently active
 * addr   = address of the current tile
 * posX   = position of tile in the X direction
 * posY   = position of tile in the Y direction
 * ports  = tiles currently active ports
*/
struct TILE {
  byte  active;
  int   addr;
  POS   pos;
  int   ports;
};



TILE tile[TILE_MAX];

//last set as default 
int addr_lst[TILE_MAX] = {0x08, 0x10, 0x18, 0x20, 0x42};

int error;
int i;
int tileID;
int x_free;
int y_free;
int tileCount;
int pinDir = B0000;

//DotStar Setup
const uint8_t MATRIX_DATA_PIN = PB10; 
const uint8_t MATRIX_CLK_PIN = PB11;
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

void setup() {
  Serial.begin(9600); // for the serial monitor

  //Input pins for direction
  pinMode(PIN_DIR_U, INPUT);
  pinMode(PIN_DIR_D, INPUT);
  pinMode(PIN_DIR_L, INPUT);
  pinMode(PIN_DIR_R, INPUT);

  for(i = 0; i < TILE_MAX; i++){
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


  Wire.begin();
  matrix.begin(); // Initialize pins for output

  matrix.setBrightness(64); // Set max brightness (out of 255)
  
  matrix.setTextWrap(false);
  matrix.setTextColor(colors[0]);
  matrix.show();  // Turn all LEDs off ASAP
}

int x    = matrix.width();
int pass = 0;
uint8_t colorIndex = 0;

void loop() {
  
  //Determine occupied directions
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

  // Loop to check if the currently existing tiles
  // still exist, if not clear and erase from layout
  // tile[0] will be reserved for the master
  tileCount = 1;
  for(i = 0; i < TILE_MAX; i++){
    if( i != 0 ){
      if( tile[i].active == 1 ){
       Wire.beginTransmission(tile[i].addr);
       error = Wire.endTransmission();
      }else{
       tileID = i;
       error = -1;
      }
    }else{
      error = 0;
      int x_cur = tile[i].pos.x;
      int y_cur = tile[i].pos.y;
      if( tile[i].ports & CNCT_U ){
        if( layout[x_cur][y_cur - 1] == 0 ) {
          x_free = x_cur;
          y_free = y_cur - 1;
        }
      }
      if( tile[i].ports & CNCT_D ){
        if( layout[x_cur][y_cur + 1] == 0 ) {
          x_free = x_cur;
          y_free = y_cur + 1 ;          
        }
      }
      if( tile[i].ports & CNCT_L ){
        if( layout[x_cur - 1][y_cur] == 0 ) {
          x_free = x_cur - 1;
          y_free = y_cur;
        }
      }
       if( tile[i].ports & CNCT_R ){
        if( layout[x_cur + 1][y_cur] == 0 ) {
          x_free = x_cur + 1;
          y_free = y_cur;
        }
      }     
    }

    if (error == SUCCESS) {
      Serial.print("I2C device found at address 0x");
      Serial.println(tile[i].addr, HEX);
      //If available request current port status from slave devices
      Wire.requestFrom(tile[i].addr, 1);
      tile[i].ports = Wire.read();
      tileCount++;
    }else if(error == -1){
      Serial.print("No device assigned at address 0x");
      Serial.println(tile[i].addr, HEX);
      tile[i].active = 0;
    }else{
      Serial.print("No I2C device found at address 0x");
      Serial.println(tile[i].addr, HEX);
      tile[i].active = 0;
    }
  }
  

  // Check if the default address exist
  Wire.beginTransmission(I2C_DEFAULT);
  error = Wire.endTransmission();
  if (error == SUCCESS){
    Serial.println("Device found at default address");
    Wire.beginTransmission(I2C_DEFAULT);
    Wire.write(tile[tileID].addr); //Assign the next available address from 
    Wire.endTransmission();  
    //Maybe insert something here during the connection process?
    Serial.print("Sent address: ");
    Serial.println(tile[tileID].addr);
  }

  //Determine the location of the tile
  Wire.beginTransmission(tile[tileID].addr);
  error = Wire.endTransmission();
  if (error == 0){
    tile[tileID].active = 1;
    tile[tileID].pos.x = x_free;
    tile[tileID].pos.y = y_free;
    layout[x_free][y_free] = tile[tileID].addr;
  }

  
  

  //TODO: Sending data dynamically

  
}
