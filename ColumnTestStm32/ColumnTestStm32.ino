// See https://learn.adafruit.com/adafruit-dotstar-leds/dotstarmatrix-library for more information

#include <Adafruit_GFX.h>
#include <Adafruit_DotStarMatrix.h>
#include <Adafruit_DotStar.h>

// Pin setup
const uint8_t MATRIX_DATA_PIN = 4;
const uint8_t MATRIX_CLK_PIN = 7;

// Size of each tile matrix
const uint8_t MATRIX_WIDTH = 4;
const uint8_t MATRIX_HEIGHT = 4;

// Number of tile matrices
const uint8_t tilesX = 1;
const uint8_t tilesY = 1;

// Last argument: line 30 in Adafruit_DotStar.h for mappings
Adafruit_DotStarMatrix matrix = Adafruit_DotStarMatrix(
  MATRIX_WIDTH, 
  MATRIX_HEIGHT, 
  tilesX, 
  tilesY,
  MATRIX_DATA_PIN, 
  MATRIX_CLK_PIN, 
  DS_MATRIX_ZIGZAG,
//  DS_TILE_ZIGZAG, 
  DOTSTAR_RGB
);

//Adafruit_DotStarMatrix matrix = Adafruit_DotStarMatrix(
//  MATRIX_WIDTH, // Width 
//  MATRIX_HEIGHT, // Height
//  MATRIX_DATA_PIN, // Data pin
//  MATRIX_CLK_PIN, // Clock pin
//  DS_MATRIX_TOP     + DS_MATRIX_LEFT +
//  DS_MATRIX_COLUMNS + DS_MATRIX_ZIGZAG,
//  DOTSTAR_RGB);

const uint8_t columnLeds[] = {0, 7, 8, 15};

const uint16_t colors[] = {
  matrix.Color(255, 0, 0), matrix.Color(0, 255, 0), matrix.Color(0, 0, 255), matrix.Color(255, 255, 255) };
  
void setup() {
  #if defined(__AVR_ATtiny85__) && (F_CPU == 16000000L)
  clock_prescale_set(clock_div_1); // Enable 16 MHz on Trinket
  #endif

  matrix.begin(); // Initialize pins for output

  matrix.setBrightness(8); // Set max brightness (out of 255)
  
  matrix.setTextWrap(false);
  matrix.setTextColor(colors[0]);
  matrix.show();  // Turn all LEDs off ASAP
  Serial.begin(9600);
}

void showColumn(uint8_t whichColumn, uint16_t color, uint8_t fps) {
  //takes in an fps
  matrix.fillScreen(0);
  matrix.setCursor(0, 0);
  matrix.drawLine(whichColumn, 0, whichColumn, MATRIX_HEIGHT, color );
//  for (uint8_t i = 0; i < sizeof(columnLeds)/sizeof(columnLeds[0]); i++) {
//    if (whichColumn % 2 == 0)
//      matrix.drawPixel(columnLeds[i] - whichColumn, i, color);
//    else
//      matrix.drawPixel(columnLeds[i] + whichColumn, i, color);
//  }
  matrix.show();
  delay(1000/fps);
}
uint8_t column = 0;
int x    = matrix.width();
int pass = 0;

void loop() {
   showColumn(column, colors[1], 2);
   column++;
   if(column > MATRIX_WIDTH) {
    column = 0;
   }
}
