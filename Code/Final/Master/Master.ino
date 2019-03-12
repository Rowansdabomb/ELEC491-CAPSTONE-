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
HardwareTimer fpsTimer(2); // timer for updating the screen (send i2c data)

MasterTile Master(0xFF);

void setup() {
  // Flag initialization
  i2cUpdateFlag = false;
  
  // Serial Setup - for output
  Serial.begin(9600); 

  /////////////////// Timer setup ////////////////////
  fpsTimer.pause();

  fpsTimer.setPeriod(1000*1000/MATRIX_FRAME_RATE); // in microseconds

  fpsTimer.setChannel1Mode(TIMER_OUTPUT_COMPARE);
  fpsTimer.setCompare(TIMER_CH1, 1);  // Interrupt 1 count after each update
  fpsTimer.attachCompare1Interrupt(i2cUpdate);

  fpsTimer.refresh();

  fpsTimer.resume();
  ///////////////////////////////////////////////////

  Serial.println("Let's Begin");
}



void loop() {
  Master.setTextData(textData, textLength);//This should be done by Sanket's code
  if(i2cUpdateFlag) {
    // Master.resetTileOrder();

    Master.handleDisplayShape();
    for(uint8_t i = 0; i < Master.getTileCount(); ++i) {
      struct POS scrollPos = Master.getScrollPos();
      uint16_t charXIndex = scrollPos.x/CHAR_WIDTH;
      struct POS outPos = Master.getOffsetCursor(i, charXIndex);

      uint8_t dataOutSize = Master.getOutputData(dataOut, textData, textLength, charXIndex);
      //tileOrder[i] is the index of the tile
      uint8_t tileID = Master.getOrderedTileID(i);
      if (tileID == MASTER_TILE_ID) {
        Master.updateTileDisplay(i, dataOut);
      } else {
        struct TILE slave = Master.getTile(tileID);
        Master.transmitToSlave(slave.addr, outPos, colors[RED], dataOut);
      }
    }
    i2cUpdateFlag = false;
    Master.updateScrollPos();
  }
}

/*
Interrupt Subroutine on a timer.
Toggles the i2cUpdateFlag at a frequency of MATRIX_FRAME_RATE
*/
void i2cUpdate() {
  i2cUpdateFlag = true;
}
