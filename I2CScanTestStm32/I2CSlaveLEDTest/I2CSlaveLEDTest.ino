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

#define I2C_ADDR  4
const int ledPin = PC13;
const int led1   = PA8;
const int led2   = PA9;
const int led3   = PA10;
int led_line = 0;

void setup()
{
  Wire.begin(I2C_ADDR);         // join i2c bus with address #2
  Wire.onRequest(requestEvent); // register event
  Wire.onReceive(receiveEvent); // register event
  pinMode(ledPin, OUTPUT);
  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);
  pinMode(led3, OUTPUT);
  Serial.begin(9600);           // start serial for output
}

void loop()
{
  //Serial.println("I2CSLAVE");
}

// function that executes whenever data is received from master
// this function is registered as an event, see setup()
void receiveEvent(int howMany)
{
  //int x = Wire.read();        // receive byte as an integer
 
  //if(x == 1){
  //  digitalWrite(ledPin,HIGH);          // print the integer
  //}else{
  // digitalWrite(ledPin, LOW);
  //}
  
  ///while(1 < Wire.available()) //loop through all but the last
  //{ 
    int led_line = Wire.read(); //receive byte as a int
  //}
  if ((led_line % 3) == 2 ){
    digitalWrite(led1, HIGH);
    digitalWrite(led2, LOW);
    digitalWrite(led3, LOW);
  }else if ( (led_line % 3) == 1){
    digitalWrite(led1,LOW);
    digitalWrite(led2, HIGH);
    digitalWrite(led3, LOW);
  }else{
    digitalWrite(led1, LOW);
    digitalWrite(led2, LOW);
    digitalWrite(led3, HIGH);
  }
}

// function that executes whenever data is requested by master
// this function is registered as an event, see setup()
void requestEvent()
{
  Wire.write("hello\n");  // respond with message of 6 bytes
                          // as expected by master
}
