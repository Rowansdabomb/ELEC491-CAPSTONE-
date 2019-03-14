#ifndef t25_colors_h
#define t25_colors_h

#include "Arduino.h"
#include "MatrixSetup.h"

uint16_t makeColor(uint8_t r, uint8_t g, uint8_t b);

const uint8_t RED = 2;
const uint8_t BLUE = 0;
const uint8_t GREEN = 1;
const uint8_t WHITE = 3;
const uint8_t ORANGE = 4;
const uint8_t VIOLET = 5;
const uint16_t colors[6] = {
  makeColor(255, 0, 0), // BLUE
  makeColor(0, 255, 0), // GREEN
  makeColor(0, 0, 255), // RED
  makeColor(255, 255, 255), //WHITE
  makeColor(0, 165, 255),  //ORANGE
  makeColor(238, 130, 238) // VIOLET
};

#endif
