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
#define MATRIX_DATA_PIN  PA4
#define MATRIX_CLK_PIN   PA7
#define BTN_PIN          PA5

int buttonState;
int lastButtonState = 0;
unsigned long lastDebounceTime = 0;
unsigned long debounceDelay = 50;

const uint8_t CHAR_WIDTH = 5;
const uint8_t CHAR_HEIGHT = 8;

// Size of each tile matrix
const uint8_t matrixWidth = 4;
const uint8_t matrixHeight = 4;


// Number of tile matrices
const uint8_t tilesX = 1;
const uint8_t tilesY = 1;

// I2C 
uint8_t I2C_ADDR = 0x69;
#define I2C_DEFAULT 0x42

// Initilaize matrix object
Adafruit_DotStarMatrix matrix = Adafruit_DotStarMatrix(
  matrixWidth, 
  matrixHeight, 
  tilesX, 
  tilesY,
  MATRIX_DATA_PIN, 
  MATRIX_CLK_PIN, 
  DS_MATRIX_TOP     + DS_MATRIX_LEFT +
  DS_MATRIX_COLUMNS + DS_MATRIX_PROGRESSIVE + DS_TILE_PROGRESSIVE,
  DOTSTAR_RGB
);

const uint16_t colors[] = {
  matrix.Color(255, 0, 0), matrix.Color(0, 255, 0), matrix.Color(0, 0, 255), matrix.Color(255, 255, 255) 
};

void showColumn(uint8_t whichColumn, uint16_t color) {
  //takes in an fps
  matrix.fillScreen(0);
  matrix.setCursor(0, 0);
  matrix.drawLine(whichColumn, 0, whichColumn, matrixHeight, colors[1] );
//  for (uint8_t i = 0; i < sizeof(columnLeds)/sizeof(columnLeds[0]); i++) {
//    if (whichColumn % 2 == 0)
//      matrix.drawPixel(columnLeds[i] - whichColumn, i, color);
//    else
//      matrix.drawPixel(columnLeds[i] + whichColumn, i, color);
//  }
  matrix.show();
}

void setup()
{
  #if defined(__AVR_ATtiny85__) && (F_CPU == 16000000L)
    clock_prescale_set(clock_div_1); // Enable 16 MHz on Trinket
  #endif

/*  matrix.begin(); //initialize pins for output
  matrix.setBrightness(8); // Set max brightness 
  matrix.setTextWrap(false);
  matrix.setTextColor(colors[0]);
  matrix.show();
*/
  //I2C Setup
  Wire.begin(I2C_DEFAULT);         // join i2c bus with the default address
  Wire.onRequest(requestEvent); // register event
  Wire.onReceive(receiveEvent); // register event
  
  //Button Setup
  pinMode(BTN_PIN, INPUT);
  
  Serial.begin(9600);           // start serial for output
}

uint8_t column = 0;

void loop()
{
   int reading = digitalRead(BTN_PIN);
  //buttonEvent
  if (reading != lastButtonState) {
      lastDebounceTime = millis();
  }
  //Serial.println(I2C_ADDR);
  if ((millis()-lastDebounceTime)> debounceDelay) {
    if (reading != buttonState){
      buttonState = reading;
      //Serial.print("button pressed but don't know value: ");
      //Serial.println(buttonState);
      if (buttonState == 1){
         //Serial.println("button press");
         //Turn off I2C and reinitialize
         Wire.end();
         delay(1000); //slight delay before trying to reintialize
         Wire.begin(I2C_ADDR); //join the i2c bus with a different address
         Wire.onRequest(requestEvent);
         Wire.onReceive(receiveEvent);
         Serial.print("Attempted Address Change");
         Serial.print(I2C_ADDR);    
      }
    }
  }
  lastButtonState = reading;
}

// function that executes whenever data is received from master
// this function is registered as an event, see setup()
void receiveEvent(int howMany)
{
  if (Wire.available()) //loop through all but the last
  { 
    I2C_ADDR = Wire.read(); //receive byte as a int
  }
  Serial.println("read value");
}

// function that executes whenever data is requested by master
// this function is registered as an event, see setup()
void requestEvent()
{
  Wire.write("hello\n");  // respond with message of 6 bytes
                          // as expected by master
}
