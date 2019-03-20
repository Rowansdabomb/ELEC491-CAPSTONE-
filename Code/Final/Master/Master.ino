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

  sensorTimer.setPeriod(SENSOR_POLL_PERIOD*1000); // in microseconds

  sensorTimer.setChannel1Mode(TIMER_OUTPUT_COMPARE);
  sensorTimer.setCompare(TIMER_CH1, 1);  // Interrupt 1 count after each update
  sensorTimer.attachCompare1Interrupt(sensorRead);

  sensorTimer.refresh();

  sensorTimer.resume();
  ///////////////////////////////////////////////////
}


void loop() {
  // master.setTextData(textData, textLength);//This should be done by Sanket's code
  if(i2cUpdateFlag) {
    master.handleDisplayShape();
    for(uint8_t i = 0; i < master.getTileCount(); ++i) {
      char dataOut[MAX_DISPLAY_CHARS];

      struct POS outPos = master.getOutputData(dataOut, i);
      outPos.y = -2;
      
      uint8_t tileID = master.getOrderedTileID(i);
      if (tileID == MASTER_TILE_ID) {
        master.updateTileDisplay(outPos, dataOut);
      } else {
        struct TILE slave = master.getTile(tileID);
        master.transmitToSlave(slave.addr, outPos, currentColor, dataOut);
      }
    }
    master.updateScrollPos();

    i2cUpdateFlag = false;
  }

  master.readSensorData();
  Serial.println(master.frameRate);
  
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
  i2cUpdateFlag = true;
}

void sensorRead() {
  master.ISR_sensorRead();
}
