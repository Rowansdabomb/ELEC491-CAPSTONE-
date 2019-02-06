#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_DotStarMatrix.h>
#include <Adafruit_DotStar.h>

#define I2C_DEFAULT 0x42

#define CNCT_U B0001
#define CNCT_D B0010
#define CNCT_L B0100
#define CNCT_R B1000

#define PIN_DIR_U PB12
#define PIN_DIR_D PB13
#define PIN_DIR_L PB14
#define PIN_DIR_R PB15

#define TILE_MAX 5

#define DEBUG 0

int print_flag = 0;

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

int error;
int i;
int tileID;
int x_free;
int y_free;
int dirChange;
int dirChange_f;
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

void handler_tim(void);

void setup() {
  Serial.begin(9600); // for the serial monitor

  //Input pins for direction
  
  pinMode(PIN_DIR_U, INPUT_PULLDOWN);
  pinMode(PIN_DIR_D, INPUT_PULLDOWN);
  pinMode(PIN_DIR_L, INPUT_PULLDOWN);
  pinMode(PIN_DIR_R, INPUT_PULLDOWN);
  
  
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
  

  //Timer for testing purposes
  Timer2.setMode(TIMER_CH1, TIMER_OUTPUTCOMPARE);
  Timer2.setPeriod(1000000);
  Timer2.setCompare(TIMER_CH1, 1);
  Timer2.attachInterrupt(TIMER_CH1, handler_tim);
  

  Wire.begin();
  /*
  matrix.begin(); // Initialize pins for output

  matrix.setBrightness(64); // Set max brightness (out of 255)
  
  matrix.setTextWrap(false);
  matrix.setTextColor(colors[0]);
  matrix.show();  // Turn all LEDs off ASAP
  */
}

int x    = matrix.width();
int pass = 0;
uint8_t colorIndex = 0;

void loop() {

  //Serial.println(print_flag);
  
  if(print_flag == 1){
  
    Serial.print("x free: ");
    Serial.print(x_free);
    Serial.print(",y_free: ");
    Serial.println(y_free);
    //Serial.print("DIR: ");
    //Serial.println(tile[0].ports, BIN);
    Serial.print("Tile ID: ");
    Serial.println(tileID);
    Serial.print("Tile 0 x: ");
    Serial.print(tile[0].pos.x);
    Serial.print(",y: ");
    Serial.print(tile[0].pos.y);
    Serial.print(",active: ");
    Serial.print(tile[0].active);
    Serial.print(", ports: ");
    Serial.println(tile[0].ports);
    
    Serial.print("Tile 1 x: ");
    Serial.print(tile[1].pos.x);
    Serial.print(",y: ");
    Serial.print(tile[1].pos.y);
    Serial.print(",active: ");
    Serial.print(tile[1].active);
    Serial.print(", ports: ");
    Serial.println(tile[1].ports);    
    
    Serial.print("Tile 2 x: ");
    Serial.print(tile[2].pos.x);
    Serial.print(",y: ");
    Serial.print(tile[2].pos.y);
    Serial.print(",active: ");
    Serial.print(tile[2].active);
    Serial.print(", ports: ");
    Serial.println(tile[2].ports);
     
    Serial.print("Tile 3 x: ");
    Serial.print(tile[3].pos.x);
    Serial.print(",y: ");
    Serial.print(tile[3].pos.y);
    Serial.print(",active: ");
    Serial.print(tile[3].active);
    Serial.print(", ports: ");
    Serial.println(tile[3].ports);

    Serial.print("Tile 4 x: ");
    Serial.print(tile[4].pos.x);
    Serial.print(",y: ");
    Serial.print(tile[4].pos.y);
    Serial.print(",active: ");
    Serial.print(tile[4].active);
    Serial.print(", ports: ");
    Serial.println(tile[4].ports);    
    
    Serial.println(); 
    print_flag = 0;
  }
  
  
  //Determine occupied directions
  tile[0].ports_pre = tile[0].ports;
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
        tileCount++;
      }else{
        if(DEBUG){
          Serial.print("No I2C device found at address 0x");
          Serial.println(tile[i].addr, HEX);
        }// END DEBUG PRINT
        tile[i].pos.x = 0;
        tile[i].pos.y = 0;
        tile[i].active = 0;
      }// END Address Successfully found
 
    }
    //Check if the directional ports has changed
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
  
  if(dirChange_f == 1 ){
    
    // Check if the default address exist
    Wire.beginTransmission(I2C_DEFAULT);
    error = Wire.endTransmission();
    Serial.println(error);
    if (error == SUCCESS){
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
    error = Wire.endTransmission();
    if (error == 0){
      tile[tileID].active = 1;
      tile[tileID].pos.x = x_free;
      x_free = 0;
      tile[tileID].pos.y = y_free;
      y_free = 0;
      layout[x_free][y_free] = tile[tileID].addr;
      dirChange_f = 0;// reset the direction changed flag
    }

  }
  

  //TODO: Sending data dynamically
  if(tile[4].active == 1){
    Wire.beginTransmission(tile[4].addr);
    switch(tile[0].ports){
      case CNCT_U:
        Wire.write('E');
        Wire.write('1');
        Wire.write('0');
        Wire.write('6');
        break;
      case CNCT_D:
        Wire.write('E');
        Wire.write('0');
        Wire.write('1');
        Wire.write('7');
        break;  
      case CNCT_L:
        Wire.write('E');
        Wire.write('1');
        Wire.write('1');
        Wire.write('8');
        break;  
      case CNCT_R:
        Wire.write('E');
        Wire.write('0');
        Wire.write('0');
        Wire.write('9');
        break; 
      default:
        Wire.write('D');
        break;
    }//End Switch
    Wire.endTransmission();    
  }
  
  delay(100);
  
}

void handler_tim(void) {
  print_flag = 1;
}
