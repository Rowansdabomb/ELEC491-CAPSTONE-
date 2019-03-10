#ifndef t25_constants_h
#define t25_constants_h

// must always be odd
const uint8_t MASTER_TILE_ID = 9;

const uint8_t MATRIX_FRAME_RATE = 4;
const uint8_t I2C_DEFAULT = 0x42;

const uint8_t TILE_MAX = 5;
const uint8_t DEBUG = 0;
const uint8_t TILE_MAP_SIZE  = 7;

const uint8_t MAX_DISPLAY_CHARS = 2;

//last set as default 
const uint8_t addr_lst[TILE_MAX] = {0x08, 0x10, 0x18, 0x20, 0x28};

#endif