#pragma once
#include "MatrixSetup.h"

const uint8_t RED = 0;
const uint8_t BLUE = 1;
const uint8_t GREEN = 2;
const uint8_t WHITE = 3;
const uint8_t ORANGE = 4;
const uint8_t VIOLET = 5;
const uint16_t colors[6] = {
  matrix.Color(255, 0, 0), matrix.Color(0, 255, 0), matrix.Color(0, 0, 255), matrix.Color(255, 255, 255), matrix.Color(255, 165, 0), matrix.Color(238, 130, 238)
};
