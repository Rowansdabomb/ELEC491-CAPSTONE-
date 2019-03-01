/***************************************************************************************
*  Title: Topology Test Slave
* Author: Jimmy Wong
* Date: February 10, 2019
* Code version: 0.0.3
***************************************************************************************/

#include <Wire_slave.h>
#include <Adafruit_GFX.h>
#include <Adafruit_DotStarMatrix.h>
#include <Adafruit_DotStar.h>

// Pin setup

#define CNCT_U B0001
#define CNCT_D B0010
#define CNCT_L B0100
#define CNCT_R B1000

#define PIN_DIR_U PA4
#define PIN_DIR_D PA6
#define PIN_DIR_L PA3
#define PIN_DIR_R PA5

#define LED_U PA3
#define LED_D PA4
#define LED_L PA5
#define LED_R PA2

//DotStar Setup
const uint8_t MATRIX_DATA_PIN = PB11; 
const uint8_t MATRIX_CLK_PIN = PB10;
const uint8_t CHAR_WIDTH = 5;
const uint8_t CHAR_HEIGHT = 8;

// Size of each tile matrix
const uint8_t matrixWidth = 4;
const uint8_t matrixHeight = 4;

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

int buttonState = 0;
// I2C 
uint8_t I2C_ADDR = 0x42; //Initalize I2C_ADDR
#define I2C_DEFAULT 0x42

int pos_x = 0;
int pos_y = 0;

int dspy_en = 0;
int mtrx_en = 0;
int brightness = 0;

int led_out[2][2] = { {LED_L, LED_U},
                      {LED_D, LED_R}};
                      
void handler_tim(void);

void setup()
{
  //I2C Setup
  Wire.begin(I2C_DEFAULT);         // join i2c bus with the default address
  Wire.onRequest(requestEvent); // register event
  Wire.onReceive(receiveEvent); // register event
  
  // Directional Pin Setup
  pinMode(PIN_DIR_U, INPUT_PULLDOWN);
  pinMode(PIN_DIR_D, INPUT_PULLDOWN);
  pinMode(PIN_DIR_L, INPUT_PULLDOWN);
  pinMode(PIN_DIR_R, INPUT_PULLDOWN);
  /*
  // Directional LED setup **Replace with DotStar** 
  pinMode(LED_U, OUTPUT);
  pinMode(LED_D, OUTPUT);
  pinMode(LED_L, OUTPUT);
  pinMode(LED_R, OUTPUT);*/

  // Timer Setup
  Timer2.setMode(TIMER_CH1, TIMER_OUTPUTCOMPARE);
  Timer2.setPeriod(1000000);
  Timer2.setCompare(TIMER_CH1, 1);
  Timer2.attachInterrupt(TIMER_CH1, handler_tim);

  // DotStar Setup - BEGIN 
  matrix.begin();
  matrix.setBrightness(64); // Set max brightness 
  matrix.setTextWrap(false);
  matrix.setTextColor(colors[0]);
  matrix.show();  // Turn all LEDs off ASAP

  //  Serial Setup - for output
  Serial.begin(9600);           
}

int ports = B0000;
int temp_ports = B0000;
uint8_t column = 0;
char msg_buf[6] = {0, 0, 0, 0, 0, 0};



void loop()
{
//  Serial.println("loop");
  if (buttonState == 1){
    //Turn off I2C and reinitialize
    Wire.begin(I2C_ADDR); //join the i2c bus with a different address
    Wire.onRequest(requestEvent);
    Wire.onReceive(receiveEvent);
    Serial.print("Attempted Address Change");
    Serial.println(I2C_ADDR, HEX); 
    buttonState = 2;   
  }
  //Determine occupied directions
  temp_ports = B0000;
  if(digitalRead(PIN_DIR_U)){
    temp_ports = temp_ports | CNCT_U;
  }
  if(digitalRead(PIN_DIR_D)){
    temp_ports = temp_ports | CNCT_D;
  }
  if(digitalRead(PIN_DIR_L)){
    temp_ports = temp_ports | CNCT_L;
  }
  if(digitalRead(PIN_DIR_R)){
    temp_ports = temp_ports | CNCT_R;
  }
  ports = temp_ports;

  //make color from 2 bytes

  uint16_t color = (msg_buf[2] << 8 ) | (msg_buf[3] & 0xff);

  // get pos_x from col and index
  unsigned char pos_x = msg_buf[0] % (msg_buf[4]*matrixWidth);
  unsigned char pos_y = msg_buf[1] % (msg_buf[4]*matrixHeight);
  
  if(mtrx_en){
//    Serial.print("Received: ");
//    Serial.println(msg_buf[0]);
    matrix.fillScreen(0);
    if((ports & CNCT_U) == CNCT_U){
      matrix.fillRect(1, 3, 2, 1, colors[msg_buf[4]]);
    }
    if((ports & CNCT_D) == CNCT_D){
      matrix.fillRect(1, 0, 2, 1, colors[msg_buf[4]]);
    }
    if((ports & CNCT_L) == CNCT_L){
      matrix.fillRect(0, 1, 1, 2, colors[msg_buf[4]]);
    }
    if((ports & CNCT_R) == CNCT_R){
      matrix.fillRect(3, 1, 1, 2, colors[msg_buf[4]]);
    }    
    matrix.fillRect( 1, 1, 2, 2, colors[2]); // mid white square
    matrix.fillRect( pos_y, pos_x, matrixHeight, 1, color);
    matrix.show();
  }

  
  delay(100);
  //Serial.println("continuing");
}

// function that executes whenever data is received from master
// this function is registered as an event, see setup()
void receiveEvent(int howMany)
{
  char c;
  Serial.println("Event Received");
  if(Wire.available()){
    c = Wire.read();      // receive first byte as a character
    Serial.println(c);         // print the character
  }
  if(c == 'A'){
    I2C_ADDR = Wire.read();    // receive byte as an integer
    Serial.println(I2C_ADDR, HEX);         // print the integer h
    if(buttonState != 2){
      Wire.end();
      buttonState = 1;
    }
  }else if (c == 'B'){
    mtrx_en = 1;
    int i = 0;
    while(Wire.available()){
      msg_buf[i] = (uint8_t) Wire.read();
      Serial.print(i);
      Serial.print(" ");
      Serial.println(msg_buf[i], BIN);
      i++;     
    }
  }
}

// function that executes whenever data is requested by master
// this function is registered as an event, see setup()
void requestEvent()
{
  Wire.write(ports);  // respond with message of 1 byte
                      // as expected by master
}
void handler_tim(void){
  digitalWrite(led_out[0][0], LOW);
  digitalWrite(led_out[1][0], LOW);
  digitalWrite(led_out[0][1], LOW);
  digitalWrite(led_out[1][1], LOW);
}
