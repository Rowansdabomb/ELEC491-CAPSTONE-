  /* Wire Slave Receiver
by Wi6Labs

Demonstrates use of the Wire library.
Receives/sends data as an I2C/TWI slave device.
Refer to the "Wire Master Reader Writer" example for use with this.

Created 27 June 2017
Updated 14 August 2017
  - this example is now common to all STM32 boards

This example code is in the public domain.
*/

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_DotStarMatrix.h>
#include <Adafruit_DotStar.h>

// Pin setup
const uint8_t MATRIX_DATA_PIN = 4;
const uint8_t MATRIX_CLK_PIN = 7;
#define LED1_PIN PA1
#define LED2_PIN PA2
#define LED3_PIN PA3
#define LED4_PIN PA4
int LED_array [4] = {LED1_PIN, LED2_PIN, LED3_PIN, LED4_PIN};

// Size of each tile matrix
const uint8_t MATRIX_WIDTH = 4;
const uint8_t MATRIX_HEIGHT = 4;

// Number of tile matrices
const uint8_t tilesX = 1;
const uint8_t tilesY = 1;

#define I2C_ADDR  2
/*
void showColumn(uint8_t whichColumn, uint16_t color) {
  //takes in an fps
  matrix.fillScreen(0);
  matrix.setCursor(0, 0);
  matrix.drawLine(whichColumn, 0, whichColumn, MATRIX_HEIGHT, color );
//  for (uint8_t i = 0; i < sizeof(columnLeds)/sizeof(columnLeds[0]); i++) {
//    if (whichColumn % 2 == 0)
//      matrix.drawPixel(columnLeds[i] - whichColumn, i, color);
//    else
//      matrix.drawPixel(columnLeds[i] + whichColumn, i, color);
//  }
  matrix.show();
}
*/
void setup()
{
  #if defined(__AVR_ATtiny85__) && (F_CPU == 16000000L)
    clock_prescale_set(clock_div_1); // Enable 16 MHz on Trinket
  #endif

 // matrix.begin(); //initialize pins for output
 // matrix.setBrightness(8); // Set max brightness 
 // matrix.setTextWrap(false);
// matrix.setTextColor(colors[0]);
//  matrix.show()
  Wire.begin(I2C_ADDR);         // join i2c bus with address #2
  Wire.onRequest(requestEvent); // register event
  Wire.onReceive(receiveEvent); // register event

  pinMode(LED1_PIN, OUTPUT);
  pinMode(LED2_PIN, OUTPUT);
  pinMode(LED3_PIN, OUTPUT);
  pinMode(LED4_PIN, OUTPUT);
  
  Serial.begin(9600);           // start serial for output
}

uint8_t column = MATRIX_WIDTH + 1;

void loop()
{
  //showColumn(column, colors[1];
  digitalWrite(LED1_PIN,LOW);
  digitalWrite(LED2_PIN,LOW);
  digitalWrite(LED3_PIN,LOW);
  digitalWrite(LED4_PIN,LOW);
  if (column < MATRIX_WIDTH){
    digitalWrite(LED_array[column],HIGH);
  }
}

// function that executes whenever data is received from master
// this function is registered as an event, see setup()
void receiveEvent(int howMany)
{
  Serial.println("receiving data: ");
  if(Wire.available()){
    column = Wire.read(); //receive byte as a int
    Serial.println(column);
  }
}

// function that executes whenever data is requested by master
// this function is registered as an event, see setup()
void requestEvent()
{
  Wire.write("hello\n");  // respond with message of 6 bytes
                          // as expected by master
}
