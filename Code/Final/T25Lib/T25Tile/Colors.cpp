#include "Colors.h"

#define max(r, g, b)

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

/*
rgbToHsl - converts rgb color to hsl color, modifies the incoming array and returns it as hsl array
  Inputs:
    rgb - array of 3 unsigned chars containing red, green, and blue color values (0 - 255)
  Outputs:
    hsl
*/
float * rgbToHsl(uint8_t rgb[]) {
  float r = rgb[0]/255;
  float g = rgb[1]/255;
  float b = rgb[2]/255;
  float _rgb[3] = {r, g, b};
  float cmax = getMax(_rgb, 3);
  float cmin = getMin(_rgb, 3);
  float delta = cmax - cmin;

  float L = (cmax + cmin) / 2;
  float S;
  if (delta = 0) {
    S = 0;
  } else {
    S = delta / (1 - abs((2 * L) - 1));
  }
  float H;
  if (delta = 0) {
    H = 0;
  } else if (cmax = r) {
    H = 60 * (((g - b)/delta) % 6);
  } else if (cmax = g) {
    H = 60 * (((b - r)/delta) + 2);
  } else {
    H = 60 * (((r - g)/delta) + 4);
  }
  
  float hsl[3];
  hsl[0] = H;
  hsl[1] = S;
  hsl[2] = L;

  return hsl;
}

/*
converts hsl to rgb colors
*/
uint8_t * hslToRgb(float hsl[]) {
  float C = (1 - abs(2 * hsl[2] - 1) * hsl[1];
  float X = C * (1 - abs((hsl[0] / 60) % 2 - 1));
  float m = hsl[2] - C/2;
  float _rgb[3];
  if (0 <= hsl[0] && hsl[0] < 60) {
    _rgb[0] = C; _rgb[1] = X; _rgb[2] = 0;
  } else if (60 <= hsl[0] && hsl[0] < 120) {
    _rgb[0] = X; _rgb[1] = C; _rgb[2] = 0;
  } else if (120 <= hsl[0] && hsl[0] < 180) {
    _rgb[0] = 0; _rgb[1] = C; _rgb[2] = X;
  } else if (180 <= hsl[0] && hsl[0] < 240) {
    _rgb[0] = 0; _rgb[1] = X; _rgb[2] = C;
  } else if (240 <= hsl[0] && hsl[0] < 300) {
    _rgb[0] = X; _rgb[1] = 0; _rgb[2] = C;
  } else if (300 <= hsl[0] && hsl[0] < 360) {
    _rgb[0] = C; _rgb[1] = 0; _rgb[2] = X;
  }

  uint8_t rgb[3];
  for (uint8_t i = 0; i < 3; i++) {
    rgb[i] = (uint8_t) (_rgb[i] + m)*255;
  }
  return rgb;
} 

float getMax(float arr[], const uint8_t size) {
  float result = 0;
  for(uint8_t i = 0; i < size; i++) {
    if(result < arr[i]) result = arr[i];
  }
  return result;
}

float getMin(float arr[], const uint8_t size) {
  float result = 255;
  for(uint8_t i = 0; i < size; i++) {
    if(result > arr[i]) result = arr[i];
  }
  return result;
}

/*
linear interpolation algo
Inputs:
  v0 - start value
  v1 - end value
  t - value between 0 and 1, 0 being v0 and 1 being v1
*/
float lerp(float v0, float v1, float t) {
  return (1 - t) * v0 + t * v1;
}

