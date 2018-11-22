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
const uint8_t MATRIX_DATA_PIN = PB10; 
const uint8_t MATRIX_CLK_PIN = PB11;
const uint8_t TEST_PIN = PA7;

const uint8_t CHAR_WIDTH = 5;
const uint8_t CHAR_HEIGHT = 8;

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
  DS_MATRIX_TOP     + DS_MATRIX_LEFT +
  DS_MATRIX_COLUMNS + DS_MATRIX_ZIGZAG + DS_TILE_PROGRESSIVE,
  DOTSTAR_RGB
);

const uint16_t colors[] = {
  matrix.Color(255, 0, 0), matrix.Color(0, 255, 0), matrix.Color(0, 0, 255), matrix.Color(255, 255, 255) 
};
  
void setup() {
//  #if defined(__AVR_ATtiny85__) && (F_CPU == 16000000L)
//  clock_prescale_set(clock_div_1); // Enable 16 MHz on Trinket
//  #endif

  pinMode(TEST_PIN, OUTPUT);

  matrix.begin(); // Initialize pins for output

  matrix.setBrightness(64); // Set max brightness (out of 255)
  
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

void scrollText(uint8_t fps, char* text, int textLength) {
  // Test Conditions:
  //  variables: matrixWidth, matrixHeight,
  //  No delay
  
  // Results: (same for text = "3 2 1" and text = "6 5 4 3 2 1")
  //  Max frame rate
  //    16x16 = 57fps
  //    4x4 = 96fps
  //  SetupPeriod
  //    16x16 = 17.5ms
  //    4x4 < 1.25ms

  
  // Takes in an fps and text to scroll
  gpio_write_bit(GPIOA, 7, HIGH);
  matrix.fillScreen(0);
  matrix.setCursor(x, 0);
  matrix.print(F(text));
//  Serial.println(x);
  if(--x < -textLength*CHAR_WIDTH) {
    x = matrix.width();
//    if(++pass >= 3) pass = 0;
//    matrix.setTextColor(colors[pass]);
  }
  matrix.show();
  gpio_write_bit(GPIOA, 7, LOW);
  delay(1000/fps);
}

void testEachPixel(uint8_t fps) {
  //takes in an fps
  matrix.fillScreen(0);
  matrix.setCursor(x, 0);
  uint16_t color = colors[0];
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

uint8_t i = 16;

void testAllPixels(uint8_t fps) {
  matrix.setCursor(0, 0);
  matrix.fillRect(0, 0, matrixWidth, 4*matrixHeight, colors[1]);
  
  Serial.println(colorIndex);
  colorIndex++;
  if (colorIndex >= 4){
    colorIndex = 0;
//    matrix.setBrightness(255/(i*2));
//    Serial.print("Color Brightness ");
//    Serial.println(256/i);
//    i*=2;
//    if(i > 16)
//      i = 1;
  }
  
  matrix.show();
  delay(1000/fps);
}

void testDigital() {
  // produces 672.4kHz square wave
  // risetime (3V) < 42ns 
  // falltime (.3V) < 47ns
  digitalWrite(PA7, HIGH);
  digitalWrite(PA7, LOW);
}

void testGPIO() {
  // produces 1.893MHz pulse wave
  // risetime (2.5V) < 18ns (rises only to 2.7V)
  // falltime (.32V) < 52ns
  
  gpio_write_bit(GPIOA, 7, HIGH);
  gpio_write_bit(GPIOA, 7, LOW);
}

void loop() {
  Serial.println(F_CPU);
  Serial.println(clock_div_1);
  // put your main code here, to run repeatedly:
 char text[] = "6 5 4 3 2 1";
// Serial.println(sizeof(text)/sizeof(text[0]));
// Serial.println(PA8);
// Serial.println(PB15);
 scrollText(10, text, sizeof(text)/sizeof(text[0]));
// delay(1);

//  testDigital();
//  testGPIO();

// Serial.println("MAIN");
//   testEachPixel(8);
//   testAllPixels(1);
}
