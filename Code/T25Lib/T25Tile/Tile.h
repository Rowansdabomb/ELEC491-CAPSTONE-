#ifndef t25_tile_h
#define t25_tile_h

#include <Arduino.h>
#include <T25Setup.h>
#include <Adafruit_GFX.h>
#include <Adafruit_DotStarMatrix.h>
#include <Adafruit_DotStar.h>

struct POS {
  int8_t x;
  int8_t y;
};

/* Tile structure
 * active = is this tile currently active
 * addr   = address of the current tile
 * posX   = position of tile in the X direction
 * posY   = position of tile in the Y direction
 * ports  = state of the directional pins
 * previousPorts = state of the directional pins in the previous instance 
*/

struct TILE {
  bool      active;
  uint8_t   addr;
  POS       pos;
  uint8_t   ports;
  uint8_t   previousPorts;
};

struct MessageData {
    struct POS pos;
    uint16_t color;
    uint8_t brightness;
    uint8_t frame;
    char text[MAX_DISPLAY_CHARS];
};

class Tile {
  public:
    // INITIALIZATION
    Tile(uint8_t addr);
    void beginTile();

    // ATTRIBUTES
    char msgBuffer[8] = {0, 0, 0, 0, 0, 0, 0, 0};
    uint16_t currentColor;
    uint8_t currentBrightness;
    uint8_t currentFrame;

    // SETTERS
    void setCursor(int8_t x, int8_t y);
    void setOperationMode(const uint8_t mode);
    void setBrightness(const uint8_t value);
    void setTargetFrameRate(const uint8_t rate);

    // GETTERS
    uint8_t getOperationMode();
    struct TILE getData();
    uint8_t getFrameRate();
    uint8_t getTargetFrameRate();

    struct TILE findNeighborTiles();
    void debugWithMatrix(const uint8_t x, const uint8_t y, const uint8_t color);
    void bootAnimation(int bootTime);
   
    void updateTileDisplay(const POS &outPos, char dataOut[]);
    void changeColor(uint8_t colors[]);
    void changeColor(uint16_t color);

    // SENSORS
    void printSensorData();
    void readSensorData();

    volatile uint8_t sensorID;
    volatile uint8_t prevSensorID;
    void ISR_sensorRead();

    // MATRIX
    Adafruit_DotStarMatrix *matrix;

  protected:
    uint8_t operationMode;
    struct TILE data;
    struct POS cursor;
    uint8_t frameRate;
    uint8_t targetFrameRate;
    bool toggleAmbient;

    uint8_t sensorRow;
    uint8_t sensorCol;
    uint16_t sensorData[MATRIX_WIDTH * MATRIX_HEIGHT];
    uint16_t sensorThreshold[MATRIX_WIDTH * MATRIX_HEIGHT];
    uint8_t sensorThresholdHistory[MATRIX_WIDTH * MATRIX_HEIGHT];
    uint8_t sensorThresholdCounter;
    
    void updateSensorThreshold(uint16_t sensorValue, uint8_t sensorIndex);

    void displayChar(const POS &pos, char dataOut[]);
    void displayMirror(bool defaultColor = true);
    void displayAmbient();
    void i2cDirectionTest(const uint16_t color);
};

#endif
