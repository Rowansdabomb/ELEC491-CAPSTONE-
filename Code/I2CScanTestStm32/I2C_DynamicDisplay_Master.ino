/***************************************************************************************
*	Title: I2C_DynamicDisplay_Master
*	Author: Jimmy Wong
*	Date: Nov 18, 2018
*	Code version: 1.0
***************************************************************************************/

#include <Wire.h>

#define I2C_ADDR  2

int x = 0;
byte LED_state = 0;
// Size of each tile matrix
const uint8_t MATRIX_WIDTH = 4;
const uint8_t MATRIX_HEIGHT = 4;

#define LED1_PIN PA1
#define LED2_PIN PA2
#define LED3_PIN PA3
#define LED4_PIN PA4
int LED_array [4] = {LED1_PIN, LED2_PIN, LED3_PIN, LED4_PIN};

unsigned int tilesX = 1;
void setup()
{
  Wire.begin();        // join i2c bus (address optional for master)
  pinMode(LED1_PIN, OUTPUT);
  pinMode(LED2_PIN, OUTPUT);
  pinMode(LED3_PIN, OUTPUT);
  pinMode(LED4_PIN, OUTPUT);
  Serial.begin(9600);  // start serial for output
}

  uint8_t column_max = 8;
  uint8_t column = 0;
  uint8_t tile = 0;
  uint8_t tile_column;

void loop()
{
  //Check if the slave device exists on the bus
  Wire.beginTransmission(2);
  int error = Wire.endTransmission();
  if(error == 0) {
    Serial.print("Device connected at address 2 - ");
    tilesX = 2;    
  }else{
    tilesX = 1;
  }
  column_max = MATRIX_WIDTH * tilesX;
  column++;
  if(column >= column_max){
    column = 0;
  }
  Serial.print(" column: ");
  Serial.print(column);
  tile_column = column % MATRIX_WIDTH;
  tile = (column - tile_column) / MATRIX_WIDTH;
  digitalWrite(LED1_PIN,LOW);
  digitalWrite(LED2_PIN,LOW);
  digitalWrite(LED3_PIN,LOW);
  digitalWrite(LED4_PIN,LOW);
  Serial.print(" tile #: ");
  Serial.print(tile);
  Serial.print(" tile_column: ");
  Serial.println(tile_column);
  switch(tile){
    case 0:
      digitalWrite(LED_array[tile_column],HIGH);
      if(tilesX == 2){
        Wire.beginTransmission(2);
        Wire.write(MATRIX_WIDTH+1);
        Wire.endTransmission();
      }
      break;
    case 1:
      Wire.beginTransmission(2);
      Wire.write(tile_column);
      Wire.endTransmission();
      Serial.println("Data Sent");
      break;
    default:
      break;
  }
  
  delay(100);
}
