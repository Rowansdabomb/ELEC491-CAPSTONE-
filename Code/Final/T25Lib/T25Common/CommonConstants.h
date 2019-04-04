#ifndef t25_common_constants_h
#define t25_common_constants_h

#include <Arduino.h>

// WIFI SLAVE ADDRESS
const uint8_t WIFI_SLAVE_ADDR = 0x0f;

const uint8_t MAX_STRING_SIZE = 255;

//ESP32 CODES
const uint8_t CHANGE_TEXT  = 1;
const uint8_t CHANGE_COLOR = 2;
const uint8_t CHANGE_OPERATION_MODE = 3;
const uint8_t CHANGE_BRIGHTNESS = 4;
const uint8_t CHANGE_SCROLL_SPEED = 5;

#endif