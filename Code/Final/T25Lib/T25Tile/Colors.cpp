#include "Colors.h"
#include <cmath>

//#define max(r, g, b)

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
void rgbToHsl(uint8_t rgb[], float hsl[]) {
  float r = rgb[0]/255.0;
  float g = rgb[1]/255.0;
  float b = rgb[2]/255.0;

  Serial.print("RGB: ");
  Serial.print(r);
  Serial.print(' ');
  Serial.print(g);
  Serial.print(' ');
  Serial.print(b);
  Serial.print(' ');
  Serial.println();


  float _rgb[3] = {r, g, b};

  float cMax = getMax(_rgb, 3);
  float cMin = getMin(_rgb, 3);
  float delta = cMax - cMin;

  float H;
  float S;
  float L;
  L = (cMax + cMin) * 0.50;

  if(delta == 0 ){ // This is likely to unhappen consider using tolerances for float
    S = 0;
    H = 0;
  }
  else{
    S = ( L > 0.5) ? (delta / (cMax + cMin)):(delta / (2 - cMax - cMin));
    if(cMax == r) H = ( g - b ) / delta;
    // Serial.print("cMax == r: ");
    // Serial.println(cMax == r);
    // Serial.print("g - b: ");
    // Serial.print( g - b );
    // Serial.print(" Delta: ");
    // Serial.println( delta );

    if(cMax == g) H = 2 + ( b - r ) / delta;
    if(cMax == b) H = 4 + ( r - g ) / delta;
    // Serial.print("H: ");
    // Serial.print(H);
    H = H * 60;
    if( H < 0){
      H = H + 360;
    }
    // Serial.print(" ");
    // Serial.println(H);
  }
  
  hsl[0] = round(H);
  hsl[1] = S;
  hsl[2] = L;

}

/*
converts hsl to rgb colors
*/
void hslToRgb(float hsl[], uint8_t rgb[]) {
  // static uint8_t rgb[3];

  float H = hsl[0] / 360.0;
  float S = hsl[1];
  float L = hsl[2];

  float temp1;
  float temp2;

  float r;
  float g;
  float b;

  if( S == 0 ) {
    rgb[0] = (uint8_t) round(L * 255);
    rgb[1] = (uint8_t) round(L * 255);
    rgb[2] = (uint8_t) round(L * 255);
  }else{
    temp1 = ( L < 0.5) ? (L * (1 + S)):(L + S - L * S);
    temp2 = 2 * L - temp1;
    r = boundRGB(H + 0.333);
    g = boundRGB(H);
    b = boundRGB(H - 0.333);    
    rgb[0] = finalizeRGB(r, temp1, temp2);
    rgb[1] = finalizeRGB(g, temp1, temp2);
    rgb[2] = finalizeRGB(b, temp1, temp2);
  }  
} 

float boundRGB(float rgb){
  if (rgb < 0){
    rgb++;
  }
  else if (rgb > 1){
    rgb--;
  }
  return rgb;
}

uint8_t finalizeRGB(float rgb, const float &t1, const float &t2){
  float finalValue;
  if( (6 * rgb) < 1 ){
    finalValue = t2 + (t1 - t2) * 6 * rgb;
  }else if((2 * rgb) < 1){
    finalValue = t1;
  }else if((3 * rgb) < 2){
    finalValue = t2 + (t1 - t2) * (0.666 - rgb) * 6;
  }else{
    finalValue = t2;
  }
  return round(finalValue * 255);
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

