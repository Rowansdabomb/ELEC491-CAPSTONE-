#include "Master.h"

uint8_t assignNewAddress(const uint8_t tileID, const uint8_t yFree, const uint8_t xFree) {
  // Check if the default address exist
  uint8_t response = -1;
  Wire.beginTransmission(I2C_DEFAULT);
  response = Wire.endTransmission();
  Serial.print("assignNewAddress FIRST ");
  Serial.println(response);
  if (response != SUCCESS) return response;
  
  Wire.beginTransmission(I2C_DEFAULT);
  Wire.write('A');
  Wire.write(tile[tileID].addr); //Assign the next available address from 
  response = Wire.endTransmission();
  if (response != SUCCESS) return response;
  Serial.println("address write success");
  
  uint8_t waits = 0;
  response = -1;
  while(response != SUCCESS && waits < 20) {
    delay(10);
    Wire.beginTransmission(tile[tileID].addr);
    response = Wire.endTransmission();
    ++waits;
  }
  Serial.print("assignNewAddress LAST ");
  Serial.println( response);
  if (response != SUCCESS) return response;
  
  tile[tileID].active = true;
  tile[tileID].pos.x = xFree;   
  tile[tileID].pos.y = yFree;  
  tileMap[yFree][xFree] = tileID;
  return SUCCESS;
}



void displayChar(POS &pos, char dataOut[]){
  matrix.fillScreen(0);
  matrix.setCursor(pos.x, pos.y);
  for(int i = 0; i < MAX_DISPLAY_CHARS; ++i){ //For 4x4 should be 2
    matrix.print(dataOut[i]);
  }
  matrix.show();
}