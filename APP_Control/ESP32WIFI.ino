#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiServer.h>
#include <WiFiUdp.h>
#include <IOXhop_FirebaseESP32.h>
//#include <FirebaseESP32.h>
#include <HardwareSerial.h>
#include <string.h>

HardwareSerial ESPSerial(1);

#define FIREBASE_HOST "https://tileapp-ff417.firebaseio.com/"              //database api url
#define FIREBASE_AUTH "hBWyw9GJqnizmPs9e9MSXW6SCaRDjUY8NGvxAqaq"           //database secret

const char* ssid = "OnePlus3";// "FreeWifi 2.4G"  OnePlus3
const char* password =  "goaway1234!";       //"goaway1234!"

// Compare with Stored Value
 String TextM= "0"; 
 String ColourM = "0";
 String temp_text = "ty";
String temp_color = "0aef5c"; 

void setup() {
   Serial.begin(115200);
   ESPSerial.begin(57600,SERIAL_8N1,16,17);
 
  WiFi.begin(ssid, password);
 
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }
 
  Serial.println("Connected to the WiFi network");
  Serial.println(WiFi.localIP());
 
//  wifiServer.begin();
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);

}

void loop() {

// For Colour Change ///////////////////////////////////////
    ColourM = Firebase.getString("/Colour");
    if (temp_color != ColourM) {
        ColourChange(ColourM);
        temp_color = ColourM; 
        
    }
    
//// To Enter Text ///////////////////////////////////////////////////////
    TextM = Firebase.getString("/Text"); 
    if (temp_text != TextM) {
        TextChange(TextM);
        temp_text = TextM; 
    
    }
}


void HextoRGB (String hexstring, int rgb[]){

long long number = strtoll( &hexstring[1], NULL, 16);

// Split them up into r, g, b values
long r = number >> 16;
long g = number >> 8 & 0xFF;
long b = number & 0xFF;

rgb[0] = r;
rgb[1] = g;
rgb[2] = b;

}

void ColourChange(String HexColourCode){
  HexColourCode = '#' + HexColourCode;
  
  int rgb[3];
  HextoRGB(HexColourCode, rgb);
  ESPSerial.write(2);
 
  for(int i  =0; i < 3; i++){
     Serial.println(rgb[i]); 
     ESPSerial.write(rgb[i]);
  } 
}

void TextChange(String TextFB) {
  TextFB = TextFB + ';';
 // Serial.println(TextFB);
  int lenght = TextFB.length(); 
  char STMTxt[lenght+1];

  strcpy(STMTxt, TextFB.c_str());  // copy String into a char Array
  
  Serial.print("Text being Sent: ");
  Serial.println(STMTxt);
  
  ESPSerial.write(1);
  ESPSerial.write(STMTxt);
}

