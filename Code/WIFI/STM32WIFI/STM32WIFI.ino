#include <Arduino.h>
#include <string.h>
#include <Wire_slave.h>

#include "T25Common.h"

uint8_t rgb[3];
char textData[MAX_STRING_SIZE];
uint8_t textDataSize;
uint8_t transmitType;
uint8_t mode;
uint8_t brightness;
uint8_t scrollSpeed;

void setup() {
    Serial.begin(115200);
    // Serial Setup - for ESP32
    Serial1.begin(115200);

    //I2C Setup
    Wire.begin(WIFI_SLAVE_ADDR);         // join i2c bus with the default address
    Wire.onRequest(requestEvent);    // register event
    Wire.onReceive(receiveEvent);  // register event

    for (uint8_t i = 0; i < 3; i++) {
        rgb[i] = 255;
    }

    textData[0] = 'H';
    textData[1] = 'I';
    textData[2] = '\0';

    textDataSize = 0;

    transmitType = CHANGE_TEXT;
}

void loop() {
  // Check for available data from ESP
//   Serial.println("running");
  if (Serial1.available() > 1) {
      // Serial.println("// Serial available");
      transmitType = Serial1.read();
      Serial.print("transmit type: ");
      Serial.println(transmitType);

      switch(transmitType) {
        case CHANGE_COLOR:
        {
            // Serial.println("Change Color");
          for(uint8_t i = 0; i < 3; i++) {
            rgb[i] = Serial1.read();
            delay(10);
          }

          break;
        }
        case CHANGE_TEXT:
        {
            Serial.println("Change Text");
          textDataSize = Serial1.readBytesUntil('\0', textData, MAX_STRING_SIZE);
          textData[textDataSize] = '\0';
            
          break;
        }
        case CHANGE_OPERATION_MODE:
        {
             Serial.println("Change Mode");
            mode = Serial1.read();
            break;
        }  
        case CHANGE_BRIGHTNESS:
        {
            Serial.println("Change Brightness");
            brightness = Serial1.read();
            Serial.println(brightness);
            break;
        }     
        case CHANGE_SCROLL_SPEED:
        {
            Serial.println("Change Scroll Speed");
            scrollSpeed = Serial1.read();
            Serial.println(scrollSpeed);
            break;
        }     
        default:
          // DO NOTHING
          break;
      }
  }
}

void requestEvent()
{
    // // Serial.println("Event Requested...");
    switch(transmitType) {
        case CHANGE_COLOR:
            Wire.write(CHANGE_COLOR);
            Wire.write(3);
            for (int i = 0; i < 3; i++) {
                Wire.write(rgb[i]);
            }
            break;
        case CHANGE_TEXT:
            Wire.write(CHANGE_TEXT);
            Wire.write(textDataSize);
            for (int i = 0; i < textDataSize; i++) {
                Wire.write(textData[i]);
            }
        break;
        case CHANGE_OPERATION_MODE:
            Wire.write(CHANGE_OPERATION_MODE);
            Wire.write(1);
            Wire.write(mode);
            break;
        case CHANGE_BRIGHTNESS:
            Wire.write(CHANGE_BRIGHTNESS);
            Wire.write(1);
            Wire.write(brightness);
            break;
        case CHANGE_SCROLL_SPEED:
            Wire.write(CHANGE_SCROLL_SPEED);
            Wire.write(1);
            Wire.write(scrollSpeed);
            break;
        default:
            // DO NOTHING
            break;
    }
}

void receiveEvent(int size){
    // // Serial.println("Event Received...");
    switch(transmitType) {
        case CHANGE_COLOR:
            Wire.write(CHANGE_COLOR);
            Wire.write(3);
            for (int i = 0; i < 3; i++) {
                Wire.write(rgb[i]);
            }
            break;
        case CHANGE_TEXT:
            Wire.write(CHANGE_TEXT);
            Wire.write(textDataSize);
            for (int i = 0; i < textDataSize; i++) {
                Wire.write(textData[i]);
            }
            break;
        case CHANGE_OPERATION_MODE:
            Wire.write(CHANGE_OPERATION_MODE);
            Wire.write(1);
            Wire.write(mode);
            break;
        case CHANGE_BRIGHTNESS:
            Wire.write(CHANGE_BRIGHTNESS);
            Wire.write(1);
            Wire.write(brightness);
            break;
        case CHANGE_SCROLL_SPEED:
            Wire.write(CHANGE_SCROLL_SPEED);
            Wire.write(1);
            Wire.write(scrollSpeed);
            break;
        default:
            // DO NOTHING
            break;
    }
}

