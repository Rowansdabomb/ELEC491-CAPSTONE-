// See https://learn.adafruit.com/adafruit-dotstar-leds/dotstarmatrix-library for more information

#include <Adafruit_GFX.h>
#include <Adafruit_DotStarMatrix.h>
#include <Adafruit_DotStar.h>


//Adafruit_DotStarMatrix matrix = Adafruit_DotStarMatrix(
//  4, 4,  // Width, height
//  4, 7, // Data pin, clock pin
//  DS_MATRIX_TOP     + DS_MATRIX_LEFT +
//  DS_MATRIX_COLUMNS + DS_MATRIX_PROGRESSIVE,
//  DOTSTAR_BGR);

// Pin setup
const uint8_t MATRIX_DATA_PIN = 4;
const uint8_t MATRIX_CLK_PIN = 7;

// Size of each tile matrix
const uint8_t matrixWidth = 4;
const uint8_t matrixHeight = 4;

// Number of tile matrices
const uint8_t tilesX = 1;
const uint8_t tilesY = 1;

// Last argument: line 30 in Adafruit_DotStar.h for mappings
Adafruit_DotStarMatrix matrix = Adafruit_DotStarMatrix(
  matrixWidth, 
  matrixHeight, 
  tilesX, 
  tilesY,
  MATRIX_DATA_PIN, 
  MATRIX_CLK_PIN, 
  DS_TILE_PROGRESSIVE, 
  DOTSTAR_RGB
);

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

int x    = matrix.width();
int pass = 0;
uint8_t p_x = 0;
uint8_t p_y = 0;
uint8_t colorIndex = 0;

void scrollText(uint8_t fps, char* text) {
  // Takes in an fps and text to scroll
  matrix.fillScreen(0);
  matrix.setCursor(x, 0);
  matrix.print(F(text));
  if(--x < -36) {
    x = matrix.width();
    if(++pass >= 3) pass = 0;
    matrix.setTextColor(colors[pass]);
  }
  matrix.show();
  delay(1000/fps);
}

void testEachPixel(uint8_t fps) {
  //takes in an fps
  matrix.fillScreen(0);
  matrix.setCursor(x, 0);
  uint16_t color = matrix.Color(255/(p_x), 255/(p_y), 255/(p_x + p_y));
  Serial.println(color);
  matrix.drawPixel(p_x, p_y, color);
  p_x++;
  if (p_x >= matrixWidth) {
    p_x = 0;
    p_y++;
  }
  if (p_y >= matrixHeight) {
    p_y = 0;
  }
  matrix.show();
  delay(1000/fps);
}

uint8_t i = 1;

void testAllPixels(uint8_t fps) {
  matrix.setCursor(0, 0);
  matrix.fillRect(0, 0, matrixWidth, matrixHeight, colors[3]);
  
  Serial.println(colorIndex);
  colorIndex++;
  if (colorIndex >= 4){
    colorIndex = 0;
//    matrix.setBrightness(255/(i*2));
    Serial.print("Color Brightness ");
    Serial.println(256/i);
    i*=2;
    if(i > 16)
      i = 1;
  }
  
  matrix.show();
  delay(1000/fps);
}

void loop() {
  // put your main code here, to run repeatedly:
// char text[] = "Hello World";
// scrollText(10, text);
//   testEachPixel(1);
   testAllPixels(1);
}
