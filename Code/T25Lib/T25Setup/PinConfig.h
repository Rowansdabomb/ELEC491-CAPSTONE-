#ifndef t25_pin_config_h
#define t25_pin_config_h

#include <Arduino.h>

// DIRECITONAL PIN CONFIGURATIONS
const uint8_t PIN_DIR_U = PA12;
const uint8_t PIN_DIR_D = PA15;
const uint8_t PIN_DIR_L = PA11;
const uint8_t PIN_DIR_R = PB3;

// DIRECTIONAL BITMASKS
const uint8_t CNCT_U = B0001;
const uint8_t CNCT_D = B0010;
const uint8_t CNCT_L = B0100;
const uint8_t CNCT_R = B1000;

// MUX OUTPUT
const uint8_t PIN_MROW_ENABLE = PB8;
const uint8_t PIN_MCOL_ENABLE = PB9;

// MUX ROW SELECTS
const uint8_t MROW_0 = PB12;
const uint8_t MROW_1 = PB13;
const uint8_t MROW_2 = PB14;

// MUX COL SELECTS
const uint8_t MCOL_0 = PB15;
const uint8_t MCOL_1 = PA8;
const uint8_t MCOL_2 = PA9; // MAKE PA9 when not using // Serial

const uint8_t COLUMN_READ_PINS[MATRIX_WIDTH] = {PA0, PA1, PA2, PA3, PA4, PA5, PA6, PA7};

#endif
