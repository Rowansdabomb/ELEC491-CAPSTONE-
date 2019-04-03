#ifndef t25_matrix_setup_h
#define t25_matrix_setup_h

#include <Arduino.h>

//DotStar Setup
const uint8_t MATRIX_DATA_PIN = PB11; 
const uint8_t MATRIX_CLK_PIN = PB10;
const uint8_t CHAR_WIDTH = 6;
const uint8_t CHAR_HEIGHT = 8;

const uint8_t MAX_BRIGHTNESS = 128;

// Size of each tile matrix
const uint8_t MATRIX_WIDTH = 8;
const uint8_t MATRIX_HEIGHT = 8;

// Number of tile matrices
const uint8_t TILES_X = 1;
const uint8_t TILES_Y = 1;

#endif

