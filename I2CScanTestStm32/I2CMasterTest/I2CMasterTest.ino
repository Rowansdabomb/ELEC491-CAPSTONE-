/* Wire Master Reader Writer
by Wi6Labs

Demonstrates use of the Wire library.
Reads/writes data from/to an I2C/TWI slave device.
Refer to the "Wire Slave Sender Receiver" example for use with this.

Created 27 June 2017
Updated 14 August 2017
  - this example is now common to all STM32 boards

This example code is in the public domain.
*/

#include <Wire.h>

#define I2C_ADDR  2

int x = 0;
byte LED_state = 0;
int led_line  = 0;
void setup()
{
  Wire.begin();        // join i2c bus (address optional for master)
  Serial.begin(9600);  // start serial for output
}

void loop()
{
  Wire.requestFrom(I2C_ADDR, 6);  // request 6 bytes from slave device

  //while(Wire.available())         // slave may send less than requested
  //{
  //  char c = Wire.read();         // receive a byte as character
  //  Serial.print(c);              // print the character
  //}
  //delay(10);
  //LED_state = x % 2;
  //Wire.beginTransmission(2); // transmit to device
  //Wire.write(LED_state);            // sends one byte
  //Wire.endTransmission();           // stop transmitting
  //Wire.beginTransmission(3); // transmit to device
  //Wire.write(LED_state);            // sends one byte
  //Wire.endTransmission();           // stop transmitting

  led_line = x % 6; 
  if(led_line < 3){
    Wire.beginTransmission(2);
    Wire.write(led_line % 3);
    Wire.endTransmission();
  }else{//3,4,5 tranmsit to device 4 and device will receive 0 1 or 2
    Wire.beginTransmission(4);
    Wire.write(led_line % 3);
    Wire.endTransmission();
  }
  x++;

  delay(500);
}
