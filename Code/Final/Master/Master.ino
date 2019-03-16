/***************************************************************************************
*  Title: Topology Test Master
* Author: Jimmy Wong, Rowan Baker-French
* Date: February 10, 2019
* Code version: 0.0.7
***************************************************************************************/

#include <Wire.h>
#include <T25Lib.h>

/* THESE ARE TEMP VARIABLES
  Ideally this should be a temp variable from Sanket's code, for now assume it's 4 for TL25;
*/
uint16_t textLength = 4;
char textData[4] = {'T','L','2', '5'};

char dataOut[MAX_DISPLAY_CHARS];

volatile bool i2cUpdateFlag;
void i2cUpdate();

HardwareTimer fpsTimer(2); // timer for updating the screen (send i2c data)

MasterTile master(0xFF);

void setup() {
  // Flag initialization
  i2cUpdateFlag = false;
  // Serial Setup - for output
  Serial.begin(9600); 
  pinMode(LED_BUILTIN, OUTPUT);
  ///////////////// Timer setup ////////////////////
  fpsTimer.pause();

  fpsTimer.setPeriod(1000*1000/master.frameRate); // in microseconds

  fpsTimer.setChannel1Mode(TIMER_OUTPUT_COMPARE);
  fpsTimer.setCompare(TIMER_CH1, 1);  // Interrupt 1 count after each update
  fpsTimer.attachCompare1Interrupt(i2cUpdate);

  fpsTimer.refresh();

  fpsTimer.resume();
  ///////////////////////////////////////////////////

  master.beginMasterTile();
}


void loop() {
  master.setTextData(textData, textLength);//This should be done by Sanket's code
  if(i2cUpdateFlag) {
    // Serial.println(master.getOperationMode()); 
    master.handleDisplayShape();
    for(uint8_t i = 0; i < master.getTileCount(); ++i) {
      // Serial.println("Before getScrollPos");
      struct POS scrollPos = master.getScrollPos();
      uint16_t charXIndex = scrollPos.x/CHAR_WIDTH;
      // Serial.println("Before getOffsetCursor");
      struct POS outPos = master.getOffsetCursor(i, charXIndex);

      // Serial.println("Before getOutputData");
      master.getOutputData(dataOut, textData, textLength, charXIndex);
      //tileOrder[i] is the index of the tile
      // Serial.println("Before getOrderedTileID");
      uint8_t tileID = master.getOrderedTileID(i);
      if (tileID == MASTER_TILE_ID) {
        // Serial.println("Before updateTileDisplay");
        master.updateTileDisplay(i, dataOut);
      } else {
        struct TILE slave = master.getTile(tileID);
        // Serial.println("Before transmitToSlave");
        master.transmitToSlave(slave.addr, outPos, colors[RED], dataOut);
      }
    }
    // Serial.println("Before handleDisplayShape");
    master.updateScrollPos();

    i2cUpdateFlag = false;
    // digitalWrite(LED_BUILTIN, HIGH);
  }
}

/*
Interrupt Subroutine on a timer.
Toggles the i2cUpdateFlag at a frequency of MATRIX_FRAME_RATE
*/
void i2cUpdate() {
  i2cUpdateFlag = true;
}
