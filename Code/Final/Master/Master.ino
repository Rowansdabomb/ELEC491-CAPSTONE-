/***************************************************************************************
*  Title: Topology Test Master
* Author: Jimmy Wong, Rowan Baker-French, Sanket Mittal
* Date: February 10, 2019
* Code version: 0.1.0
***************************************************************************************/

#include <Wire.h>
#include "T25Tile.h"
#include "T25MasterTile.h"

volatile bool i2cUpdateFlag;
void i2cUpdate();

void sensorRead();

HardwareTimer fpsTimer(2); // timer for updating the screen (send i2c data)
HardwareTimer sensorTimer(3); // time for polling the sensors

MasterTile master(0xFF);

uint16_t currentColor = colors[RED];
uint8_t currentFrame = 1;

void setup() {
  // Flag initialization
  i2cUpdateFlag = false;
  // Serial Setup - for output
  Serial.begin(57600); 
  // Serial Setup - for ESP32
  Serial1.begin(57600);

  ///////////////// TILE BEGIN /////////////////////
  master.beginMasterTile();

  ///////////////// MATRIX FPS TIMER SETUP ////////////////////
  fpsTimer.pause();

  fpsTimer.setPeriod(1000*1000/master.frameRate); // in microseconds

  fpsTimer.setChannel1Mode(TIMER_OUTPUT_COMPARE);
  fpsTimer.setCompare(TIMER_CH1, 1);  // Interrupt 1 count after each update
  fpsTimer.attachCompare1Interrupt(i2cUpdate);

  fpsTimer.refresh();

  fpsTimer.resume();

  ///////////////// SENSOR POLL TIMER SETUP ////////////////////
  sensorTimer.pause();

  sensorTimer.setPeriod(SENSOR_POLL_PERIOD); // in microseconds

  sensorTimer.setChannel1Mode(TIMER_OUTPUT_COMPARE);
  sensorTimer.setCompare(TIMER_CH1, 1);  // Interrupt 1 count after each update
  sensorTimer.attachCompare1Interrupt(sensorRead);

  sensorTimer.refresh();

  sensorTimer.resume();
  ///////////////////////////////////////////////////
}

void loop() {
  if(i2cUpdateFlag) {
    master.handleDisplayShape();

    if (currentFrame % (master.frameRate / master.scrollSpeed) == 0) {
      for(uint8_t i = 0; i < master.getTileCount(); ++i) {
        char dataOut[MAX_DISPLAY_CHARS];

        struct POS outPos = master.getOutputData(dataOut, i);
        outPos.y = -2;
        
        uint8_t tileID = master.getOrderedTileID(i);


        //   uint8_t rgb1[3] = {255, 0, 0};
        //   uint8_t rgb2[3] = {0, 255, 0};
        //   float * hsl1;
        //   float * hsl2;
          
        //   hsl1 = rgbToHsl(rgb1);
        //   hsl2 = rgbToHsl(rgb2);
        //   float h = lerp(hsl1[0], hsl2[0], (float) currentFrame / master.frameRate);
          
        //   hsl1[0] = h;
        //   uint8_t *rgb;
        //   rgb = hslToRgb(hsl1);

          // master.changeColor(makeColor(rgb[0], rgb[1], rgb[2]));


          if (tileID == MASTER_TILE_ID) {
            master.updateTileDisplay(outPos, dataOut);
          } else {
            struct TILE slave = master.getTile(tileID);
            master.transmitToSlave(slave.addr, outPos, currentColor, dataOut);
          }
        }
      master.updateScrollPos();
    }

    
    i2cUpdateFlag = false;
  }


  master.readSensorData();
  
  // Check for available data from ESP
  if(Serial1.available() > 1) {
      int transmitType = Serial1.read();

      switch(transmitType) {
        case CHANGE_COLOR:
        {
          uint8_t rgb[3];

          for(uint8_t i = 0; i < 3; i++) {
            rgb[i] = Serial1.read();
            delay(10);
          }

          currentColor = makeColor(rgb[0], rgb[1], rgb[2]);
          master.changeColor(currentColor);
          break;
        }
        case CHANGE_TEXT:
        {
          char textData[MAX_STRING_SIZE];
          int textDataSize = Serial1.readBytesUntil('\0', textData, MAX_STRING_SIZE);
          textData[textDataSize] = '\0';
            
          master.setTextData(textData, textDataSize);//This should be done by Sanket's code
          break;
        }
        case CHANGE_OPERATION_MODE:
        {
          master.setOperationMode(Serial1.read());
        }

        default:
          // DO NOTHING
          break;
      }
  }
}

/*
Interrupt Subroutine on a timer.
Toggles the i2cUpdateFlag at a frequency of MATRIX_FRAME_RATE
*/
void i2cUpdate() {
  ++currentFrame;
  if (currentFrame > 30) currentFrame = 1;
  i2cUpdateFlag = true;
}

void sensorRead() {
  master.ISR_sensorRead();
}
