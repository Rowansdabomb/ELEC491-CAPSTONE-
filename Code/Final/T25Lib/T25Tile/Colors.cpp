#include "Colors.h"

uint16_t makeColor(uint8_t r, uint8_t g, uint8_t b) {
  switch(COLOR_ORDER) {
    case RGB:
      // taken from Adafruit_dotstartmatrix.h
      return  ((uint16_t)(r & 0xF8) << 8) |
              ((uint16_t)(g & 0xFC) << 3) |
                         (b         >> 3);
    case BGR:
      return  ((uint16_t)(b & 0xF8) << 8) |
              ((uint16_t)(g & 0xFC) << 3) |
                         (r         >> 3);
  }
}

