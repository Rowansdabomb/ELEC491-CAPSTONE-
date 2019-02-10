/***************************************************************************************
*  Title: Topology Test 
* Author: Jimmy Wong
* Date: January 21, 2019
* Code version: 0.0.1
***************************************************************************************/

#include <Wire_slave.h>

// Pin setup

#define CNCT_U B0001
#define CNCT_D B0010
#define CNCT_L B0100
#define CNCT_R B1000

#define PIN_DIR_U PB12
#define PIN_DIR_D PB13
#define PIN_DIR_L PB14
#define PIN_DIR_R PB15

int buttonState = 0;
// I2C 
uint8_t I2C_ADDR = 0x42; //Initalize I2C_ADDR
#define I2C_DEFAULT 0x42

void setup()
{
  //I2C Setup
  Wire.begin(I2C_DEFAULT);         // join i2c bus with the default address
  Wire.onRequest(requestEvent); // register event
  Wire.onReceive(receiveEvent); // register event
  
  //Button Setup
 // pinMode(BTN_PIN, INPUT);
  
  Serial.begin(9600);           // start serial for output
}

int ports = B0000;
int temp_ports = B0000;
uint8_t column = 0;

void loop()
{
  Serial.println(buttonState);
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
  delay(50);
  //Serial.println("continuing");
}

// function that executes whenever data is received from master
// this function is registered as an event, see setup()
void receiveEvent(int howMany)
{
  Serial.println("Event Received");
  while(Wire.available()){
    char c = Wire.read();      // receive first byte as a character
    Serial.println(c);         // print the character
    I2C_ADDR = Wire.read();    // receive byte as an integer
    Serial.println(I2C_ADDR, HEX);         // print the integer h
    if(buttonState != 2){
      Wire.end();
      buttonState = 1;
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
