/***************************************************************************************
*	Title: I2C_DynamicAddress_Slave
*	Author: Jimmy Wong
*	Date: Nov 18, 2018
*	Code version: 0.0.1
***************************************************************************************/

#include <Wire.h>

// Pin setup
#define BTN_PIN          PA5

int buttonState;
int lastButtonState = 0;
unsigned long lastDebounceTime = 0;
unsigned long debounceDelay = 50;

// Size of each tile matrix
const uint8_t matrixWidth = 4;
const uint8_t matrixHeight = 4;


// Number of tile matrices
const uint8_t tilesX = 1;
const uint8_t tilesY = 1;

// I2C 
uint8_t I2C_ADDR = 0x21; //Initalize I2C_ADDR
#define I2C_DEFAULT 0x42

void setup()
{
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
  if ((millis()-lastDebounceTime)> debounceDelay) {
    if (reading != buttonState){
      buttonState = reading;
      //Serial.print("button pressed but don't know value: ");
      //Serial.println(buttonState);
      if (buttonState == 1){
         //Turn off I2C and reinitialize
         Wire.end();
         delay(100); //slight delay before trying to reintialize
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
