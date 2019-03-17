#ifndef t25_pin_config_h
#define t25_pin_config_h

#include "Arduino.h"

// PIN CONFIGURATIONS
const uint8_t PIN_DIR_U = PA4;
const uint8_t PIN_DIR_D = PA6;
const uint8_t PIN_DIR_L = PA3;
const uint8_t PIN_DIR_R = PA5;

// DIRECTIONAL BITMASKS
const uint8_t CNCT_U = B0001;
const uint8_t CNCT_D = B0010;
const uint8_t CNCT_L = B0100;
const uint8_t CNCT_R = B1000;

#endif
