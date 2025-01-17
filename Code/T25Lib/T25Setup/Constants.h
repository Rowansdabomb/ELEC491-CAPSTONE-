#ifndef t25_constants_h
#define t25_constants_h

#include <Arduino.h>
#include "PinConfig.h"

// must always be odd
const uint8_t MASTER_TILE_ID = 9;

const uint8_t I2C_DEFAULT = 0x42;

const float thresholdError = 1.04;

// SENSOR POLLING
const uint8_t MUX_SELECT_SIZE = 3;
const uint8_t MUX_ROW_SELECT[MUX_SELECT_SIZE] = {MROW_0, MROW_1, MROW_2}; 
const uint8_t MUX_COL_SELECT[MUX_SELECT_SIZE] = {MCOL_0, MCOL_1, MCOL_2}; 

const int SENSOR_POLL_PERIOD = 650;

// TILE
const uint8_t TILE_MAX = 5;
const uint8_t DEBUG = 0;
const uint8_t TILE_MAP_SIZE  = 7;

const uint8_t MAX_DISPLAY_CHARS = 2;

//last set as default 
const uint8_t addr_lst[TILE_MAX] = {0x08, 0x10, 0x18, 0x20, 0x28};

// Used for tile with rotated matrix (tile 1 should be true)
const bool ROTATED_MATRIX = false;
#endif