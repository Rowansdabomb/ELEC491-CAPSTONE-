#pragma once
#include <Adafruit_GFX.h>
#include <Adafruit_DotStarMatrix.h>
#include <Adafruit_DotStar.h>

//DotStar Setup
const uint8_t MATRIX_DATA_PIN = PB11; 
const uint8_t MATRIX_CLK_PIN = PB10;
const uint8_t CHAR_WIDTH = 5;
const uint8_t CHAR_HEIGHT = 8;

// Size of each tile matrix
const uint8_t matrixWidth = 4;
const uint8_t matrixHeight = 4;

// Number of tile matrices
const uint8_t tilesX = 1;
const uint8_t tilesY = 1;

Adafruit_DotStarMatrix matrix = Adafruit_DotStarMatrix(
  matrixWidth, 
  matrixHeight, 
  tilesX, 
  tilesY,
  MATRIX_DATA_PIN, 
  MATRIX_CLK_PIN, 
  DS_MATRIX_BOTTOM     + DS_MATRIX_LEFT +
  DS_MATRIX_COLUMNS + DS_MATRIX_ZIGZAG + DS_TILE_PROGRESSIVE,
  DOTSTAR_RGB
);

