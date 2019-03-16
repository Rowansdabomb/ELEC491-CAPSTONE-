#ifndef t25_constants_h
#define t25_constants_h

#include "Arduino.h"

// must always be odd
const uint8_t MASTER_TILE_ID = 9;

const uint8_t I2C_DEFAULT = 0x42;

const uint8_t TILE_MAX = 5;
const uint8_t DEBUG = 0;
const uint8_t TILE_MAP_SIZE  = 7;

const uint8_t MAX_DISPLAY_CHARS = 2;

const uint8_t MAX_STRING_SIZE = 255;

//last set as default 
const uint8_t addr_lst[TILE_MAX] = {0x08, 0x10, 0x18, 0x20, 0x28};

//OPERATION MODES
const uint8_t SCROLL_MODE = 0;
const uint8_t GESTURE_MODE = 1;
const uint8_t DIRECTION_TEST = 2;

#endif