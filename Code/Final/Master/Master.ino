/***************************************************************************************
*  Title: Topology Test Master
* Author: Jimmy Wong, Rowan Baker-French, Sanket Mittal
* Date: February 10, 2019
* Code version: 0.1.0
***************************************************************************************/

#include <Wire.h>
#include "T25Setup.h"
#include "T25Tile.h"
#include "T25MasterTile.h"
#include "T25Common.h"

volatile bool newFrameFlag;
void newFrame();

void sensorRead();

HardwareTimer fpsTimer(2); // timer for updating the screen (send i2c data)
HardwareTimer sensorTimer(3); // time for polling the sensors

MasterTile master(0xFF);

uint16_t currentColor = colors[RED];
uint8_t currentFrame = 1;

void setup() {
  // Flag initialization
  newFrameFlag = false;
  // Serial Setup - for output
  Serial.begin(115200); 
  // Serial Setup - for ESP32
  Serial1.begin(115200);

  ///////////////// TILE BEGIN /////////////////////
  master.beginMasterTile();

  ///////////////// MATRIX FPS TIMER SETUP ////////////////////
  fpsTimer.pause();

  fpsTimer.setPeriod(1000*1000/master.frameRate); // in microseconds

  fpsTimer.setChannel1Mode(TIMER_OUTPUT_COMPARE);
  fpsTimer.setCompare(TIMER_CH1, 1);  // Interrupt 1 count after each update
  fpsTimer.attachCompare1Interrupt(newFrame);

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
  pinMode(PIN_MCOL_ENABLE, OUTPUT);
  pinMode(PIN_MROW_ENABLE, OUTPUT);

  digitalWrite(PIN_MCOL_ENABLE, HIGH);
  digitalWrite(PIN_MROW_ENABLE, LOW);
  master.setOperationMode(MIRROR_MODE);
}

void loop() {
  master.readSensorData();

  if(newFrameFlag) {
    master.handleDisplayShape();

    // if (currentFrame % (master.frameRate) == 0) {
    //   Serial.println("Try database update");
    //   master.updateFromDataBase();
    // }

    if (currentFrame % (master.frameRate / master.targetFrameRate) == 0) {
      for(uint8_t i = 0; i < master.getTileCount(); ++i) {
        char dataOut[MAX_DISPLAY_CHARS];

        struct POS outPos = master.getOutputData(dataOut, i);
        
        uint8_t tileID = master.getOrderedTileID(i);
        if (tileID == MASTER_TILE_ID) {
          Serial.println("output to display");
          master.updateTileDisplay(outPos, dataOut);
        } else {
          // struct TILE slave = master.getTile(tileID);
          // master.transmitToSlave(slave.addr, outPos, currentColor, dataOut);
        }
      }
      master.updateScrollPos();
    }

    newFrameFlag = false;
  }

}

/*
Interrupt Subroutine on a timer.
Toggles the newFrameFlag at a frequency of master.frameRate
*/
void newFrame() {
  ++currentFrame;
  if (currentFrame > 30) currentFrame = 1;
  newFrameFlag = true;
}

void sensorRead() {
  master.ISR_sensorRead();
}
