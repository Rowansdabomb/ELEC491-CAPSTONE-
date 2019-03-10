#pragma once
#include "MatrixSetup.h"

const uint8_t RED = 2;
const uint8_t BLUE = 0;
const uint8_t GREEN = 1;
const uint8_t WHITE = 3;
const uint8_t ORANGE = 4;
const uint8_t VIOLET = 5;
const uint16_t colors[6] = {
  matrix.Color(255, 0, 0), // BLUE
  matrix.Color(0, 255, 0), // GREEN
  matrix.Color(0, 0, 255), // RED
  matrix.Color(255, 255, 255), //WHITE
  matrix.Color(0, 165, 255),  //ORANGE
  matrix.Color(238, 130, 238) // VIOLET
};
