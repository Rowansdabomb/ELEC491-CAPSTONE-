
#include <HardwareSerial.h>
#include <Adafruit_GFX.h>
#include <Adafruit_DotStarMatrix.h>
#include <Adafruit_DotStar.h>

// #include "Adafruit_APDS9960.h"
// Adafruit_APDS9960 apds;

// Pin setup
const uint8_t MATRIX_DATA_PIN = PB8; //PB10
const uint8_t MATRIX_CLK_PIN = PB9;  //PB11

// Size of each tile matrix
const uint8_t matrixWidth = 4;
const uint8_t matrixHeight = 4;

// Number of tile matrices
const uint8_t tilesX = 1;
const uint8_t tilesY = 1;

// Array for RBG designation
int colors[4];

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
  DOTSTAR_BGR
);

char TextforTile[50];
int Casevar;
byte b1;
int lenght;
char* temp; 

void setup() {
  // // APDS 9960 Setup ------------------------------------------------------------------------
  // #if defined(__AVR_ATtiny85__) && (F_CPU == 16000000L)
  // clock_prescale_set(clock_div_1); // Enable 16 MHz on Trinket
  // #endif
  
  // if(!apds.begin()){
  //   Serial.println("failed to initialize device! Please check your wiring.");
  // }
  // else Serial.println("Device initialized!");
   
  // //gesture mode will be entered once proximity mode senses something close
  // apds.enableProximity(true);
  // apds.enableGesture(true);

  //-----------------------------------------------------------------------------------------
  matrix.begin(); // Initialize pins for output
  matrix.setBrightness(64); // Set max brightness (out of 255)

  matrix.setTextWrap(false);
  matrix.setTextColor(matrix.Color(0,0,0));
  matrix.show();  // Turn all LEDs off ASAP
  
  Serial1.begin(57600);
  Serial.begin(57600);
  
}

void loop() {
  /// printText(TextforTile, lenght);
//ChangeColour(colors); 
if (Serial1.available()>1){
      Casevar = Serial1.read();
      Serial.print("CaseVariable: ");
      Serial.print(Casevar);
             
       if (Casevar == 2){
         Serial.println("Colors data from ESP32: "); 
             for(int i =0; i <3; i++) {
                colors[i] = Serial1.read();
                Serial.println(colors[i]);        
             }
           ChangeColour(colors);
        }
       
        else {
           b1 = Serial1.readBytesUntil(';',TextforTile,50);
              
           Serial.print("Text Recieved from ESP32: ");
           Serial.println(TextforTile);
            
           lenght = (int) b1;
           printText(TextforTile, lenght);
        }
}
      
//     //read a gesture from the device
//     uint8_t gesture = apds.readGesture();

//     switch (gesture){

//       case(APDS9960_DOWN):
//           lenght = 1; 
//           Serial.println("DOWN");
//           delay(200);
//       break;

//      case(APDS9960_UP):
//         lenght = 2; 
//         Serial.println("UP");
//       break;

      
//       default:
//           printText(TextforTile, lenght);
//           ChangeColour(colors); 
//       break;
//  }
     
}

void printText(char* text, int textlenght){
 // delay(500);
 // Serial.print("TEXT LENGHT: ");
 // Serial.println(textlenght);
  
if(textlenght == 1) {
    matrix.fillScreen(0);
    matrix.setCursor(0,0);
    
   // Serial.print("Printing Text to Tile:  ");
    temp = text;
    Serial.println(temp);
   // matrix.print(F(text));  -- OG CODE
      matrix.print(F(temp));
    matrix.setTextColor(matrix.Color(colors[0],colors[1],colors[2]));
  }
 else if (textlenght > 1) matrix.fillRect(0, 0, matrixWidth, 4*matrixHeight,matrix.Color(colors[0],colors[1],colors[2])); 
}

void ChangeColour (int colors[]) {  
    matrix.setCursor(0, 0);
    matrix.Color(colors[0],colors[1],colors[2]); 
    matrix.show();
}




/// BACKUPPPPP ///////////////////////////////////////////////////////////////////////////////////////


//            b1=Serial1.read();  //Read Upper byte
//            b2=Serial1.read();  //Read Lower byte
//            rec=(b1*256)+b2; 
        //    colors[i] = rec;
            
////  }

/*  
while (Serial1.available()>3){
  Casevar = Serial1.read();
 
  Serial.print("CaseVar: "); 
  Serial.println(Casevar);
  
    switch (Casevar) {
    case 1:  // Reading String & Printing Text to Tile 
        b1 = Serial1.readBytesUntil(';',TextforTile,50);
        
        Serial.print("Text Recieved from ESP32: ");
        Serial.println(TextforTile);
        
        lenght = (int) b1;
        printText(TextforTile, lenght);
    break;
    
    case 2:  // Reading Decimal Values & Changing Colour on screen
        //   if(Serial1.available()>3){
            Serial.println("Colors data from ESP32: "); 
            for(int i =0; i <3; i++) {
               colors[i] = Serial1.read();
               Serial.println(colors[i]);        
             }
              ChangeColour(colors);
          // }
    break;
    
    default:
     // Serial.println("DEFAULT");
    break;
    }
  }
 */ 
