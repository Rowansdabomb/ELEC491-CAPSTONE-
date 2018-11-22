#define SENSOR_POLL_PERIOD 100

#include <Adafruit_GFX.h>
#include <Adafruit_DotStarMatrix.h>
#include <Adafruit_DotStar.h>

// Pin setup
const uint8_t MATRIX_DATA_PIN = PB10;
const uint8_t MATRIX_CLK_PIN = PB11;

// Size of each tile matrix
const uint8_t matrixWidth = 4;
const uint8_t matrixHeight = 4;

// Number of tile matrices
const uint8_t tilesX = 1;
const uint8_t tilesY = 1;

Adafruit_DotStarMatrix matrix = Adafruit_DotStarMatrix(
  matrixWidth, 
  matrixHeight, 
  tilesX, 
  tilesY,
  MATRIX_DATA_PIN, 
  MATRIX_CLK_PIN, 
  DS_MATRIX_TOP     + DS_MATRIX_LEFT +
  DS_MATRIX_COLUMNS + DS_MATRIX_ZIGZAG + DS_TILE_PROGRESSIVE,
  DOTSTAR_BGR
);

const uint16_t colors[] = {
  matrix.Color(255, 0, 0), matrix.Color(0, 255, 0), matrix.Color(0, 0, 255), matrix.Color(255, 255, 255), matrix.Color(0,0,0)
};

const uint8_t sensorDataSize = 16;
volatile int sensorData[sensorDataSize] = {0};

const uint8_t led = PC13;
const uint8_t testPin = PB12;

volatile bool interruptFlag = false;

HardwareTimer timer(2);

void setup() {
  
  #if defined(__AVR_ATtiny85__) && (F_CPU == 16000000L)
  clock_prescale_set(clock_div_1); // Enable 16 MHz on Trinket
  #endif
  
  matrix.begin(); // Initialize pins for output
  matrix.setBrightness(64); // Set max brightness (out of 255)

  pinMode(led, OUTPUT);
  pinMode(testPin, OUTPUT);
  Serial.begin(38400);
  // set sensor polling interupt routine
  // Pause the timer while we're configuring it
  timer.pause();

  // Set up period
  timer.setPeriod(SENSOR_POLL_PERIOD*1000); // in microseconds

  // Set up an interrupt on channel 1
  timer.setChannel1Mode(TIMER_OUTPUT_COMPARE);
  timer.setCompare(TIMER_CH1, 1);  // Interrupt 1 count after each update
  timer.attachCompare1Interrupt(sensorRead);

  // Refresh the timer's count, prescale, and overflow
  timer.refresh();

  // Start the timer counting
  timer.resume();
}

void sensorRead() {
  uint8_t si = 0;
  uint8_t pin = 0;
  gpio_write_bit(GPIOB, 12, HIGH);
  for(uint8_t i = (sensorDataSize - 1); i > 5; --i) {
    pin = (sensorDataSize - 1) - i;
    if(pin < 8){
      sensorData[i] = analogRead(pin)/16;
    }
    else {
      sensorData[i] = analogRead(pin + 10)/16;
    }
  }
 
  gpio_write_bit(GPIOB, 12, LOW);
  interruptFlag = true;
}

void printSensorData() {
  uint16_t color;
  matrix.setCursor(0, 0);
  
  for (uint8_t i = 0; i < matrixWidth; i++) {
    for(uint8_t j = 0; j < matrixHeight; j++) {
        Serial.print(sensorData[i + j*(matrixHeight)]);

        if(j < matrixHeight - 1)
          Serial.print(" | ");
        else
          Serial.println();
      }
  }
  Serial.println();
}
int Color_Brightness(int sensorData){
  uint16_t color = matrix.Color(sensorData, 0, 0); 
  return color; 
}

void  LEDControl() {
    uint16_t color;
    int adjusted;
     for (uint8_t i = 0; i < matrixWidth; i++) {
      for(uint8_t j = 0; j < matrixHeight; j++) {
        adjusted = sensorData[i + j*(matrixHeight)];
        if(adjusted > 20)
          adjusted *= 8;
        if (adjusted > 255)
          adjusted = 255;
        color = Color_Brightness(adjusted);
        matrix.drawPixel(i, j, color);
        matrix.show();
//        matrix.fillRect(0, 0, matrixWidth, matrixHeight, color);
//        Serial.println(color);
        }
     }
//     
//    int avg = 0;
//    for(uint8_t i = 0; i < sensorDataSize; i++) {
//      avg += sensorData[i];
//    }
//    avg = avg/sensorDataSize;
//    
//    color = Color_Brightness(avg);
//    matrix.fillRect(0, 0, matrixWidth, matrixHeight, color);
//    matrix.show();
}

//Simply a while loop that gets the sensor poll data and prints it on update
void loop() {
  // put your main code here, to run repeatedly:

   
    if (interruptFlag){

      
      //print the sensor data 
      printSensorData();

      LEDControl(); 
      
      //reset flag
      interruptFlag = false;
    }
  }

