#ifndef t25_colors_h
#define t25_colors_h

#include "Arduino.h"
#include "MatrixSetup.h"

const uint8_t RGB = 0;
const uint8_t BGR = 1;
const uint8_t COLOR_ORDER = RGB; 

uint16_t makeColor(uint8_t r, uint8_t g, uint8_t b);
float * rgbToHsl(uint8_t rgb[]);
uint8_t * hslToRgb(float hsl[]);
float nGetMax(float arr[], const uint8_t size);
float nGetMin(float arr[], const uint8_t size);
float lerp(float v0, float v1, float t);

const uint8_t RED = 0;
const uint8_t GREEN = 1;
const uint8_t BLUE = 2;
const uint8_t WHITE = 3;
const uint8_t ORANGE = 4;
const uint8_t VIOLET = 5;
const uint16_t colors[6] = {
  makeColor(255, 0,   0  ), // RED
  makeColor(0,   255, 0  ), // GREEN
  makeColor(0,   0,   255), // BLUE
  makeColor(255, 255, 255), // WHITE
  makeColor(255, 165, 0  ), // ORANGE
  makeColor(238, 130, 238)  // VIOLET
};

#endif
