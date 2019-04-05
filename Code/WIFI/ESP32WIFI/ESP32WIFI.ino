#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiServer.h>
#include <WiFiUdp.h>
#include <IOXhop_FirebaseESP32.h>
#include <HardwareSerial.h>
#include <string.h>
// #include "T25Setup.h"

//ESP32 CODES
const uint8_t CHANGE_TEXT  = 1;
const uint8_t CHANGE_COLOR = 2;
const uint8_t CHANGE_OPERATION_MODE = 3;
const uint8_t WIFI_SLAVE_ADDR = 0x0f;

HardwareSerial ESPSerial(1);

#define FIREBASE_HOST "https://tileapp-ff417.firebaseio.com/"              //database api url
#define FIREBASE_AUTH "hBWyw9GJqnizmPs9e9MSXW6SCaRDjUY8NGvxAqaq"           //database secret

const char* ssid = "OnePlus3";// "FreeWifi 2.4G"  OnePlus3
const char* password =  "goaway1234!";       //"goaway1234!"

// Compare with Stored Value
String TextM           = "0"; 
String ColorM          = "0";
String ModeM           = "0";
String BrightnessM     = "0";
String ScrollM         = "0";
String temp_text       = "0";
String temp_color      = "ffffff"; 
String temp_mode       = "0";
String temp_brightness = "0";
String temp_scroll     = "0";

// volatile uint8_t mode = 0;

void setup() {
  Serial.begin(115200);
  ESPSerial.begin(115200,SERIAL_8N1,16,17);
 
  WiFi.begin(ssid, password);
 
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    // Serial.println("Connecting to WiFi..");
  }
 
  // Serial.println("Connected to the WiFi network");
  // Serial.println(WiFi.localIP());
 
//  wifiServer.begin();
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);

}

void loop() {

// For colour Change ///////////////////////////////////////
    ColorM = Firebase.getString("/Colour");
    if (temp_color != ColorM) {
        colorChange();
        temp_color = ColorM; 
    }
    
//// To Enter Text ///////////////////////////////////////////////////////
    TextM = Firebase.getString("/Text"); 
    if (temp_text != TextM) {
        textChange();
        temp_text = TextM; 
    }
//// To Enter Mode ///////////////////////////////////////////////////////
    ModeM = Firebase.getString("/Mode"); 
    if (temp_mode != ModeM) {
        modeChange();
    }

//// To Enter Mode ///////////////////////////////////////////////////////
    BrightnessM = Firebase.getString("/Brightness"); 
    if (temp_brightness != BrightnessM) {
        brightnessChange();
    }

//// To Enter Mode ///////////////////////////////////////////////////////
    ScrollM = Firebase.getString("/Speedoftext"); 
    if (temp_scroll != ScrollM) {
        speedOfTextChange();
    }
}


void HextoRGB (uint8_t rgb[]) {

  // long long number = strtoll( &hexstring[1], NULL, 16);
  long long number = strtoll( &ColorM[0], NULL, 16);

  // Split them up into r, g, b values
  long r = number >> 16;
  long g = number >> 8 & 0xFF;
  long b = number & 0xFF;

  rgb[0] = r;
  rgb[1] = g;
  rgb[2] = b;
}

void colorChange(){
  uint8_t rgb[3];
  HextoRGB(rgb);
  // Serial.print("Color change: ");
  ESPSerial.write(CHANGE_COLOR);
  for(int i  =0; i < 3; i++){
    //  Serial.print(rgb[i]);
    //  Serial.print(", "); 
     ESPSerial.write(rgb[i]);
  } 
  // Serial.println();
}

void textChange() {
  // Serial.print("Text change: ");
  // Serial.println(TextM);

  ESPSerial.write(CHANGE_TEXT);
  ESPSerial.write(TextM.c_str());
}

void modeChange() {
  // Serial.print("Mode Change: ");
  // Serial.println(ModeM);

  ESPSerial.write(CHANGE_OPERATION_MODE);
  ESPSerial.write(ModeM.c_str());
}

void brightnessChange() {
  ESPSerial.write(CHANGE_BRIGHTNESS);
  ESPSerial.write(BrightnessM.c_str());
}

void speedOfTextChange() {
  ESPSerial.write(CHANGE_SCROLL_SPEED);
  ESPSerial.write(ScrollM.c_str());
}