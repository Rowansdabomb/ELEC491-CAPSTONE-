#ifndef t25_pin_config_h
#define t25_pin_config_h

#include "Arduino.h"

// DIRECITONAL PIN CONFIGURATIONS
const uint8_t PIN_DIR_U = PA4;
const uint8_t PIN_DIR_D = PA6;
const uint8_t PIN_DIR_L = PA1;
const uint8_t PIN_DIR_R = PA5;

// DIRECTIONAL BITMASKS
const uint8_t CNCT_U = B0001;
const uint8_t CNCT_D = B0010;
const uint8_t CNCT_L = B0100;
const uint8_t CNCT_R = B1000;

// MUX ROW SELECTS
const uint8_t MROW_0 = PB12;
const uint8_t MROW_1 = PB13;
const uint8_t MROW_2 = PB14;

// MUC COL SELECTS
const uint8_t MCOL_0 = PB15;
const uint8_t MCOL_1 = PA8;
const uint8_t MCOL_2 = PA9;

#endif
