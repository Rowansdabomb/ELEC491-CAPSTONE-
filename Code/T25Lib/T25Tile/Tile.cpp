#include <Arduino.h>
#include "Tile.h"
#include "Constants.h"
#include "PinConfig.h"
#include "Colors.h"
#include "CommonConstants.h"

Tile::Tile(uint8_t addr) {
  // initialize operationMode
  operationMode = SCROLL_MODE;

  //SET ADDRESS
  data.addr = addr;

  // SET FRAME RATE
  frameRate = 30;

  // SET SCROLL SPEED
  targetFrameRate = 6;
  currentFrame = 1;

  toggleAmbient = false;
  sensorThresholdCounter = 0;

  // SENSOR SETUP
  sensorRow = 0;
  sensorCol = 1;
  sensorID = 0;
  prevSensorID = MATRIX_WIDTH * MATRIX_HEIGHT - 1;

  for (uint16_t i = 0; i < MATRIX_WIDTH*MATRIX_HEIGHT; ++i) {
    sensorData[i] = 0;
    sensorThreshold[i] = 160;
    sensorThresholdHistory[i] = 0;
  }

  //initialize the matrix
  if (ROTATED_MATRIX) {
    matrix = new Adafruit_DotStarMatrix(
      MATRIX_WIDTH,
      MATRIX_HEIGHT,
      TILES_X,
      TILES_Y,
      MATRIX_DATA_PIN,
      MATRIX_CLK_PIN,
      DS_MATRIX_TOP + DS_MATRIX_RIGHT  +
      DS_MATRIX_COLUMNS + DS_MATRIX_ZIGZAG + DS_TILE_PROGRESSIVE,
      DOTSTAR_BGR
    );
  } else {
    matrix = new Adafruit_DotStarMatrix(
      MATRIX_WIDTH,
      MATRIX_HEIGHT,
      TILES_X,
      TILES_Y,
      MATRIX_DATA_PIN,
      MATRIX_CLK_PIN,
      DS_MATRIX_TOP  + DS_MATRIX_LEFT   +
      DS_MATRIX_ROWS + DS_MATRIX_ZIGZAG + DS_TILE_PROGRESSIVE,
      DOTSTAR_BGR
    );
  }

}

void Tile::beginTile() {
  cursor.x = 0;
  cursor.y = 0;

  currentColor = makeColor(255, 255, 255);

  // DotStar Setup
  matrix->begin(); // Initialize pins for output
  matrix->setBrightness(MAX_BRIGHTNESS); // Set max brightness (out of 255)
  matrix->setTextWrap(false);
  matrix->setTextColor(colors[0]);
  matrix->show();  // Turn all LEDs off ASAP

  // ADC SETUP
  for (uint8_t i = 0; i < 8; ++i) {
    pinMode(COLUMN_READ_PINS[i], INPUT_ANALOG);
  }

  // Directional Pin Setup
  pinMode(PIN_DIR_U, INPUT_PULLDOWN);
  pinMode(PIN_DIR_D, INPUT_PULLDOWN);
  pinMode(PIN_DIR_L, INPUT_PULLDOWN);
  pinMode(PIN_DIR_R, INPUT_PULLDOWN);

  // ROW MUX SETUP
  pinMode(MROW_0, OUTPUT);
  pinMode(MROW_1, OUTPUT);
  pinMode(MROW_2, OUTPUT);
  pinMode(PIN_MROW_ENABLE, OUTPUT);
  digitalWrite(PIN_MROW_ENABLE, LOW);

  // COLUMN MUX SETUP
  pinMode(MCOL_0, OUTPUT);
  pinMode(MCOL_1, OUTPUT);
  pinMode(MCOL_2, OUTPUT);
  pinMode(PIN_MCOL_ENABLE, OUTPUT);
  digitalWrite(PIN_MCOL_ENABLE, LOW);

  bootAnimation(800);
}

/*
setCursor - sets the cursor relative to the top left of the tile matrix
  Inputs:
    x - the horizontal offset from the left edge of the tile matrix
    y - the veritcal offset from the top edge of the tile matrix
*/
void Tile::setCursor(int8_t x, int8_t y) {
  cursor.x = x;
  cursor.y = y;
}

/*
setOperationMode -
  Inputs:
    mode - an integer to determine the operation mode of the tile
  Outputs:
    void
*/
void Tile::setOperationMode(const uint8_t mode) {
  // Serial.print("set operation mode to ");
  // Serial.println(mode);
  operationMode = mode;

  matrix->setRotation(0);
  // sets the speeds for display updates
  switch(mode) {
    case (SCROLL_MODE):
      
      targetFrameRate = 6;
      break;
    case (MIRROR_MODE):
      if (ROTATED_MATRIX) matrix->setRotation(1);
      targetFrameRate = 10;
      break;
    case (AMBIENT_MODE):
      targetFrameRate = 15;
      break;
    default:
      targetFrameRate = frameRate;
  }
}

/*
setBrightness -
  Inputs:
    value - an integer to determine the tile matrix brightness
  Outputs:
    void
*/
void Tile::setBrightness(const uint8_t value) {
  currentBrightness = value;
  matrix->setBrightness(currentBrightness);
}

/*
setTargetFrameRate -
  Inputs:
    rate - an integer to determine the scroll rate of text
  Outputs:
    void
*/
void Tile::setTargetFrameRate(const uint8_t rate) {
  targetFrameRate = rate;
}

/*
getFrameRate - retrieves the target frame rate
  Inputs:
    void
  Outputs:
    frameRate - the max frame rate the tile operates at
*/
uint8_t Tile::getFrameRate() {
  return frameRate;
}

/*
getTargetFrameRate - retrieves the target frame rate
  Inputs:
    void
  Outputs:
    targetFrameRate - the current framerate to update the display with
*/
uint8_t Tile::getTargetFrameRate() {
  return targetFrameRate;
}

/*
getOperationMode - returns the current operation mode
  Inputs:
    void
  Outputs:
    operationMode - the currently set operationMode
*/
uint8_t Tile::getOperationMode() {
  return operationMode;
}

/*
getData - retrieves the Tile's status, address, position, and neighborTiles
  Inputs:
    void
  Outputs:
    TILE - a struct describing the tile data
*/
struct TILE Tile::getData() {
  return data;
}

/*
updateTileDisplay - selects proper method to update display based on current operation mode
  Inputs:
    outPos - the x and y coordinate of the display offset
    dataOut - an array containing data to be sent to the display
  Outputs:
    void
*/
void Tile::updateTileDisplay(const POS &outPos, char dataOut[]) {
  matrix->fillScreen(0);

  switch(operationMode) {
    case SCROLL_MODE:
      // Turn off muxes
      digitalWrite(PIN_MCOL_ENABLE, LOW);
      digitalWrite(PIN_MROW_ENABLE, HIGH);

      displayChar(outPos, dataOut);
      break;
    case MIRROR_MODE:
      // Turn on muxes
      digitalWrite(PIN_MCOL_ENABLE, HIGH);
      digitalWrite(PIN_MROW_ENABLE, LOW);
      displayMirror();
      break;
    case SCROLL_MIRROR_MODE:
      // Turn on muxes
      digitalWrite(PIN_MCOL_ENABLE, HIGH);
      digitalWrite(PIN_MROW_ENABLE, LOW);

      displayChar(outPos, dataOut);
      displayMirror(false);
      break;
    case AMBIENT_MODE:
      digitalWrite(PIN_MCOL_ENABLE, HIGH);
      digitalWrite(PIN_MROW_ENABLE, LOW);
      displayAmbient();
      displayMirror(false);
      break;
  }

  matrix->show();
}

/*
displayChar - Shows the visible portion of characters on the matrix
  Inputs:
    dataOut - char array of characters to be displayed
  Outputs:
    void
*/
void Tile::displayChar(const POS &pos, char dataOut[]){
  matrix->setCursor(pos.x, pos.y);
  for (int i = 0; i < MAX_DISPLAY_CHARS; ++i) { //For 4x4 should be 2
    matrix->print(dataOut[i]);
  }
}

/*
displayMirror - Lights up LEDs beneath activated sensors
  Inputs:
    defaultColor - a boolean value to determine which color to use for mirror mode
  Outputs:
    void
*/
void Tile::displayMirror(bool defaultColor) {
  for (uint8_t i = 0; i < MATRIX_WIDTH * MATRIX_HEIGHT; ++i) {
    // const uint8_t SENSOR_THRESHOLD = 145;
    if (sensorData[i] > sensorThreshold[i]) {
      const uint8_t pixel_x = (i % MATRIX_WIDTH);
      const uint8_t pixel_y = (i / MATRIX_HEIGHT);
      if(!defaultColor) {
        matrix->setBrightness((uint8_t) currentBrightness * 1.5);
        matrix->fillRect(pixel_x, pixel_y, 1, 1, makeColorComplement(currentColor));
      } else {
        matrix->fillRect(pixel_x, pixel_y, 1, 1, currentColor);
      }
    }
  }
}

/*
displayAmbient - A simple pulsating light effect
  Inputs:
    void
  Outputs:
    void
*/
void Tile::displayAmbient() {
  float modifier;
  uint8_t speed = targetFrameRate * 2;
  // uint8_t rgb[3] = {255, 255, 255};

  if (toggleAmbient) {
    modifier = (currentFrame % speed) / (speed * 1.0);
    modifier = .5 + (.5 - .5 * modifier);
  } else {
    modifier = speed - ((currentFrame % speed) / (speed * 1.0));
    modifier = .5 + (1 - .5 * modifier);
  }

  // for (uint8_t i = 0; i < 3; ++i) {
  //   rgb[i] = (int) (modifier * rgb[i]);
  // }
  // matrix->fillRect(0, 0, MATRIX_WIDTH, MATRIX_HEIGHT, makeColor(rgb[0], rgb[1], rgb[2]));
  matrix->fillRect(0, 0, MATRIX_WIDTH, MATRIX_HEIGHT, currentColor);
  matrix->setBrightness(currentBrightness * modifier);

  if (currentFrame % speed == 0) {
    toggleAmbient = !toggleAmbient;
  }
}

/*
i2cDirectionTest - test to show direction of tiles added
  Inputs:

*/
void Tile::i2cDirectionTest(const uint16_t color) {
    matrix->fillScreen(0);
    if((data.ports & CNCT_U) == CNCT_U){
      matrix->fillRect(1, 3, 2, 1, color);
    }
    if((data.ports & CNCT_D) == CNCT_D){
      matrix->fillRect(1, 0, 2, 1, color);
    }
    if((data.ports & CNCT_L) == CNCT_L){
      matrix->fillRect(0, 1, 1, 2, color);
    }
    if((data.ports & CNCT_R) == CNCT_R){
      matrix->fillRect(3, 1, 1, 2, color);
    }
    matrix->fillRect(1, 1, 2, 2, colors[WHITE]);
    matrix->show();
}

/*
findNeighborTiles - checks each port of the tile for a neighbor and updates the Tile's data
  Outputs:
    struct TILE - data including updated ports of the current tile and previous ports of current tile
*/
// void getOccupiedDirections() {
struct TILE Tile::findNeighborTiles() {
  // remember previous ports
  data.previousPorts = data.ports;

  // get current ports
  data.ports = 0b0000;
  if (digitalRead(PIN_DIR_U)) {
    data.ports = data.ports | CNCT_U;
  }
  if (digitalRead(PIN_DIR_D)) {
    data.ports = data.ports | CNCT_D;
  }
  if (digitalRead(PIN_DIR_L)) {
    data.ports = data.ports | CNCT_L;
  }
  if (digitalRead(PIN_DIR_R)) {
    data.ports = data.ports | CNCT_R;
  }

  return data;
}

/*
debugWithMatrix - Displays single pixel on tile matrix corresponding to some error code
  Inputs:
    x - horizontal offset from left edge of tile matrix
    y - veritcal offset from top edge of tile matrix
    color - predefined color code value
  Outputs:
    void
*/
void Tile::debugWithMatrix(const uint8_t x, const uint8_t y, const uint8_t color) {
  matrix->fillScreen(0);
  matrix->fillRect(x, y, 2, 2, colors[color]);
  matrix->show();
  // delay(250);
}
/*
bootAnimation - Displays a short boot animation on startup of a tile
  Inputs:
    bootTime - the total length of time for the boot
  Outputs:
    Void
*/
void Tile::bootAnimation(int bootTime){
  matrix->fillScreen(0);
  matrix->fillRect(0, 0, 4, 4, colors[RED]);
  matrix->show();
  delay(bootTime/4);
  matrix->fillRect(4, 0, 4, 4, colors[GREEN]);
  matrix->show();
  delay(bootTime/4);
  matrix->fillRect(4, 4, 4, 4, colors[YELLOW]);
  matrix->show();
  delay(bootTime/4);
  matrix->fillRect(0, 4, 4, 4, colors[BLUE]);
  matrix->show();
  delay(bootTime/4);
}

/*
changeColor - a method to change the color of the matrix
  Inputs:
    colors - an array containing the values for red, green, blue from 0-255
  Outputs:
    void
*/
void Tile::changeColor(uint8_t colors[]) {
    uint16_t newColor = makeColor(colors[0], colors[1], colors[2]);
    matrix->setCursor(0, 0);
    matrix->setTextColor(newColor);
    matrix->show();
    currentColor = newColor;
}

/*
changeColor - a method to change the color of the matrix
  Inputs:
    color - a 2byte color
  Outputs:
    void
*/
void Tile::changeColor(uint16_t color) {
    matrix->setCursor(0, 0);
    matrix->setTextColor(color);
    matrix->show();
    currentColor = color;
}

/*
  Inputs:
    void
  Outputs:
    void
*/
void Tile::printSensorData() {
  Serial.write(27);       // ESC command
  Serial.print("[2J");    // clear screen command
  Serial.write(27);
  Serial.print("[H");     // cursor to home command

  for (uint8_t i = 0; i < MATRIX_WIDTH; ++i) {
    for(uint8_t j = 0; j < MATRIX_HEIGHT; ++j) {
    // for(uint8_t j = 0; j < 1; ++j) {
      Serial.print(sensorData[j + i*(MATRIX_HEIGHT)]);
      if(j < MATRIX_HEIGHT - 1)
        Serial.print(" | ");
      else
        Serial.println();
    }
  }

  Serial.println();
}

/*
readSensorData - Goes through sensor/emitter matrix left to right, then top down.
                 Reads the current sensor, then turns off current emitter and turns on next emitter
  Inputs:
    void
  Outputs:
    void
*/
void Tile::readSensorData() {
  if (sensorID != prevSensorID) {

    // read sensor data, pin map should be 0-7 for A0-A7 so we use sensorCol
    uint16_t sensorValue = analogRead(COLUMN_READ_PINS[sensorCol]);
    uint8_t sensorIndex = sensorCol + sensorRow*MATRIX_WIDTH;

    // set the sensorData
    sensorData[sensorIndex] = sensorValue;

    updateSensorThreshold(sensorValue, sensorIndex);

    ++sensorCol;
    // // Turn off selected column
    // if (sensorCol == 7) {
    //   sensorCol++;
    // }

    if (sensorCol > MATRIX_HEIGHT - 1) {
      sensorCol = 0;
      // turn on next emitter
      ++sensorRow;
      if (sensorRow > MATRIX_WIDTH - 1) {
        sensorRow = 0;
      }
    }

    for (uint8_t i = 0; i < MUX_SELECT_SIZE; ++i) {
      if ((sensorRow >> i) & 1) {
        digitalWrite(MUX_ROW_SELECT[i], HIGH);
      } else {
        digitalWrite(MUX_ROW_SELECT[i], LOW);
      }

      if ((sensorCol >> i) & 1) {
        digitalWrite(MUX_COL_SELECT[i], HIGH);
      } else {
        digitalWrite(MUX_COL_SELECT[i], LOW);
      }
    }

    // if (sensorID == 0) {
    //   printSensorData();
    // }

    prevSensorID = sensorID;
  }

}

/*
updateSensorThreshold - checks if the sensor threshold should update
  Input:
    sensorValue - the value of the current sensor
    sensorIndex - the index of the current sensor in the sensor matrices
  Outputs:
    void
*/

void Tile::updateSensorThreshold(uint16_t sensorValue, uint8_t sensorIndex) {
    if (sensorThresholdCounter == 0) {
      // advance the theshold history
      sensorThresholdHistory[sensorIndex] = sensorThresholdHistory[sensorIndex] << 1;

      // check if sensor above threshold
      if (sensorValue > (uint16_t) (sensorThreshold[sensorIndex] / thresholdError) - 1) {

        // set the LSB high
        sensorThresholdHistory[sensorIndex] = sensorThresholdHistory[sensorIndex] | 0x0001;

        // check if sensor has been consistently above threshold
        if (sensorThresholdHistory[sensorIndex] == 0xff) {
          sensorThresholdHistory[sensorIndex] = 0;
          sensorThreshold[sensorIndex] = (uint16_t) sensorValue;
        }
      }
      // check if sensor below threshold
      else if (sensorValue * thresholdError < sensorThreshold[sensorIndex]) {
        // set sensorThreshold to new minimum
        sensorThreshold[sensorIndex] = (uint16_t) sensorValue * thresholdError;
      }
    }
    ++sensorThresholdCounter;
}

/*

*/
void Tile::ISR_sensorRead() {
  prevSensorID = sensorID;
  ++sensorID;
  if (sensorID >= MATRIX_WIDTH * MATRIX_HEIGHT) {
    sensorID = 0;
  }
}