/***************************************************************************************
*	Title: I2C_DynamicAddress_Slave
*	Description: Modification of i2c_scanner example for STM32
*	Author: Jimmy Wong
*	Date: Nov 18, 2018
*	Code version: 0.0.1
***************************************************************************************/

#include <Wire.h>
#define I2C_DEFAULT 0x42

void setup() {

  Serial.begin(9600);
  Wire.begin();
  Serial.println("\nI2C Scanner");

}

void loop() {
  byte error, address;
  int nDevices;

  Serial.println("Scanning...");

  nDevices = 0;
  for(address = 1; address < 127; address++) {
    // The i2c_scanner uses the return value of
    // the Write.endTransmisstion to see if
    // a device did acknowledge to the address.
    Wire.beginTransmission(address);
    error = Wire.endTransmission();
    
    if (error == 0) {
      Serial.print("I2C device found at address 0x");
      if (address < 16) 
        Serial.print("0");
      Serial.println(address, HEX);
      if (address == I2C_DEFAULT){
        Wire.beginTransmission(address);
        Wire.write(0x6A);
        int data_t = Wire.endTransmission();
        Serial.print("sending data ");
        Serial.print(address);
        Serial.print(" result: ");
        Serial.println(data_t);
      }

      nDevices++;
    }
    else if (error == 4) {
      Serial.print("Unknown error at address 0x");
      if (address < 16) 
        Serial.print("0");
      Serial.println(address, HEX);
    }    
  }
  if (nDevices == 0)
    Serial.println("No I2C devices found");
  else
    Serial.println("done");

  delay(5000);           // wait 5 seconds for next scan
}
