/***************************************************************************************
*  Title: Topology Test Master
* Author: Jimmy Wong
* Date: February 10, 2019
* Code version: 0.0.4
***************************************************************************************/

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_DotStarMatrix.h>
#include <Adafruit_DotStar.h>

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
  matrix.Color(0, 0, 255), matrix.Color(0, 255, 0), matrix.Color(255, 0, 0), matrix.Color(255, 255, 255) 
};

void handler_tim(void);

void show_tile_info(int tileID);

//Check Master Tile ports
int check_mstr_ports();

void send_addr(int addr);

void ud_a_dim(TILE&, int& x_min, int& x_max, int& y_min, int& y_max);
void ud_free_pos(TILE&, int &x, int& y, int &dir);
void ud_tile_ord(int &x_min, int &x_max, int &y_min, int &y_max );

void dbug_print(bool tile_info_f, bool tile_ord_f,bool a_full_f,bool a_dim_f);
void show_cnct(int m_ports);

void setup() {
  // Directional Pin Setup
  
  pinMode(PIN_DIR_U, INPUT_PULLDOWN);
  pinMode(PIN_DIR_D, INPUT_PULLDOWN);
  pinMode(PIN_DIR_L, INPUT_PULLDOWN);
  pinMode(PIN_DIR_R, INPUT_PULLDOWN);
  
  // Internal Device Map - Initial Population
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
  
  /*
  //Timer for testing purposes
  Timer2.setMode(TIMER_CH1, TIMER_OUTPUTCOMPARE);
  Timer2.setPeriod(1000000);
  Timer2.setCompare(TIMER_CH1, 1);
  Timer2.attachInterrupt(TIMER_CH1, handler_tim);
  */
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
}

int x    = matrix.width();
int pass = 0;
uint8_t colorIndex = 0;

void loop() {

  if(print_flag == PRINT_EN){
    dbug_print(false, false, false, false)
  }
  
  int array_x_max = 3;
  int array_y_max = 3;
  int array_x_min = 3;
  int array_y_min = 3;
  
  //Serial.println("Determining Directions");
  
  //Determine occupied directions
  tile[0].ports_pre = tile[0].ports;
  tile[0].ports = B0000;
  tile[0].ports = check_mstr_ports();

  // Loop to check if the currently existing tiles
  // still exist, if not clear and erase from layout
  // tile[0] will be reserved for the master
  tile_count = 1;

  //Serial.println("First I2C Check");
  Wire.beginTransmission(I2C_DEFAULT);
  int chk_error = Wire.endTransmission();
  if (chk_error == 0){
    Serial.println("Default Address still detected");
  }

  
  for(i = 0; i < TILE_MAX; i++){
    int error;
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
        ud_a_dim(tile[i], array_x_min, array_x_max, array_y_min, array_y_max); 
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
    if(tile[i].ports != tile[i].ports_pre){
      dirChange_f = 1;
      dirChange = tile[i].ports ^ tile[i].ports_pre;
      ud_free_pos(tile[i], x_free, y_free, dirChange);
    }//End of Directional ports changing
  }// End FOR loop
  
  if(dirChange_f == 1 ){
    
    // Check if the default address exist
    Wire.beginTransmission(I2C_DEFAULT);
    int def_error = Wire.endTransmission();
    if (def_error == SUCCESS){
      send_addr(tile[tileID].addr);
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
  
  if(tile_order_f == 1){ // only needs to be done if number of tiles changes
    ud_tile_ord(array_x_min, array_x_max, array_y_min, array_y_max);
    tile_order_f = 0;
  }// END if 
  
  //tile_order[x] is the index of the tile
  for(show_tile = 0; show_tile < tile_count; show_tile++){
    
    if(tile_order[show_tile] == 9){
      matrix.fillScreen(0);
      show_cnct(tile[0].ports);
      matrix.fillRect(1,1, 2, 2, colors[show_tile]);
      matrix.show();
    }else{
      Wire.beginTransmission(tile[tile_order[show_tile]].addr);
      Wire.write('B');
      Wire.write(show_tile);
      Wire.endTransmission();
    }
  }

  Serial.println("Reached the end");
  
  delay(100);
  
}

void handler_tim(void) {
  print_flag = 1;
}
/*show_tile_info: Shows current information about the tile
Inputs:
  tileID: the tile whose info we want to show
Outputs:
  Displays current tile, x position, y position, if active, and currently active ports
*/
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

//Function check_mstr_ports
//Checks the ports of the master. Requires no inputs
// Output is a int value that is specifies all the connected pins 
// E.g 1010 = up and left connected
int check_mstr_ports(){
  int ports = 0;;
  if(digitalRead(PIN_DIR_U)){
    ports = ports | CNCT_U;
  }
  if(digitalRead(PIN_DIR_D)){
    ports = ports | CNCT_D;
  }
  if(digitalRead(PIN_DIR_L)){
    ports = ports | CNCT_L;
  }
  if(digitalRead(PIN_DIR_R)){
    ports = ports | CNCT_R;
  }
  return ports;
}
/*
*up_a_dim: Update array's max and min dimensions so we don't 
*have to traverse the whole array
*inputs: 
*values are passed by reference
*outputs:
*no explicit outputs but array minimum and maximum dimensions should be updated.
*/
void ud_a_dim(TILE &tile, int &x_min, int &x_max, int &y_min, int &y_max){
  if(tile.pos.x < x_min){
    x_min = tile.pos.x;
  }
  if(tile.pos.x > x_max){
    x_max = tile.pos.x;
  }
  if(tile.pos.y < y_min){
    y_min = tile.pos.y;
  }
  if(tile.pos.y > y_max){
    y_max = tile.pos.y;
  }
}// end ud_a_dim

/*
ud_free_pos: Update the free position
Inputs: (passed by reference)
tile = tile struct that is currently being used
x = the to be updated free x position
y = the to be updated free y position
*/
void ud_free_pos(TILE &tile, int &x, int &y, int &dir){
  switch(dir){
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
      Serial.print("Error direction changed but unable to determine which direction changed");
      break;
  }
}// End ud_free_pos

/*
dbug_print: Prints out debug messages
Inputs:
tile_info_f:  flag to show current tile variables
tile_ord_f:   flag to show current tile order
a_full_f:     flag to show master's internal array state
a_dim_f:      flag to show the max and min of active tile area in the array
*/
void dbug_print(bool tile_info_f, bool tile_ord_f, bool a_full_f, bool a_dim_f){
  if(tile_info_f){
    Serial.print("x free: ");
    Serial.print(x_free);
    Serial.print(",y_free: ");
    Serial.println(y_free);
    
    show_tile_info(0);
    show_tile_info(1);
    show_tile_info(2);
    show_tile_info(3);
    show_tile_info(4); 
  }
  if(tile_ord_f){
    Serial.print("# Tiles: ");
    Serial.print(tile_count);
    Serial.print(" Order : ");
    for(int j = 0; j < 4; j++){
      Serial.print(tile_order[j]);
      Serial.print(" "); 
    }
    Serial.println();
  }
  if(a_full_f){
        Serial.println("Current Internal Array");
    for(int j = 0; j < ARRAY_SIZE; j++){
      for(int k = 0; k < ARRAY_SIZE; k++){
        Serial.print(layout[j][k]);
        Serial.print(" ");
      }
      Serial.println();
    }
  }
  if(a_dim_f){
    Serial.print("Array X Values: ");
    Serial.print(array_x_min);
    Serial.print(" ");
    Serial.println(array_x_max);
    Serial.print("Array Y Values: ");
    Serial.print(array_y_min);
    Serial.print(" ");
    Serial.println(array_y_max);
  }
}

/*
send_addr: Send Slave device with default address a new address
Input:
  addr: Address we will be sending to the slave device
Output:
  None
*/
void send_addr(int addr){
      Serial.println("Device found at default address");
      Wire.beginTransmission(I2C_DEFAULT);
      Wire.write('A');
      Wire.write(addr); //Assign the next available address from 
      Wire.endTransmission();  
      //Maybe insert something here during the connection process?
      Serial.print("Sent address: ");
      Serial.println(addr, HEX);
}
/*
show_cnct: Give a visual display of the currently connected ports
Input:
  ports: currently active ports of the tile
Output:
  DotStar should output the direction it has been connected on
*/
void show_cnct(int m_ports){
      if((m_ports & CNCT_U) == CNCT_U){
        matrix.fillRect(1, 3, 2, 1, colors[3]);
      }
      if((m_ports & CNCT_D) == CNCT_D){
        matrix.fillRect(1, 0, 2, 1, colors[3]);
      }
      if(m_ports & CNCT_L) == CNCT_L){
        matrix.fillRect(0, 1, 1, 2, colors[3]);
      }
      if((m_ports & CNCT_R) == CNCT_R){
        matrix.fillRect(3, 1, 1, 2, colors[3]);
      }
}

/*ud_tile_ord
*/
void ud_tile_ord(int &x_min, int &x_max, int &y_min, int &y_max ){
    int cnt_x;
    int cnt_y;
    int cnt_order = 0;
    for(cnt_order = 0; cnt_order < 4; cnt_order++){
          tile_order[cnt_order] = 0;  //reset the order
    }
    cnt_order = 0;
    for(cnt_y = y_min; cnt_y <= y_max; cnt_y++){
      for(cnt_x = x_min; cnt_x <= x_max; cnt_x++){
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
    }// End looping through array
}