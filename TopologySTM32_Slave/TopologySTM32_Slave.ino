/***************************************************************************************
*  Title: Topology Test Slave
* Author: Jimmy Wong
* Date: January 21, 2019
* Code version: 0.0.1
***************************************************************************************/

#include <Wire_slave.h>

// Pin setup

#define CNCT_U B0001
#define CNCT_D B0010
#define CNCT_L B0100
#define CNCT_R B1000

#define PIN_DIR_U PB12
#define PIN_DIR_D PB13
#define PIN_DIR_L PB14
#define PIN_DIR_R PB15

#define LED_U PA3
#define LED_D PA4
#define LED_L PA5
#define LED_R PA2

int buttonState = 0;
// I2C 
uint8_t I2C_ADDR = 0x42; //Initalize I2C_ADDR
#define I2C_DEFAULT 0x42

int pos_x = 0;
int pos_y = 0;

int dspy_en = 0;
int brightness = 0;

int led_out[2][2] = { {LED_L, LED_U},
                      {LED_D, LED_R}};
void handler_tim(void);

void setup()
{
  //I2C Setup
  Wire.begin(I2C_DEFAULT);         // join i2c bus with the default address
  Wire.onRequest(requestEvent); // register event
  Wire.onReceive(receiveEvent); // register event
  
  //Button Setup
  pinMode(PIN_DIR_U, INPUT_PULLDOWN);
  pinMode(PIN_DIR_D, INPUT_PULLDOWN);
  pinMode(PIN_DIR_L, INPUT_PULLDOWN);
  pinMode(PIN_DIR_R, INPUT_PULLDOWN);

  pinMode(LED_U, OUTPUT);
  pinMode(LED_D, OUTPUT);
  pinMode(LED_L, OUTPUT);
  pinMode(LED_R, OUTPUT);

  Timer2.setMode(TIMER_CH1, TIMER_OUTPUTCOMPARE);
  Timer2.setPeriod(1000000);
  Timer2.setCompare(TIMER_CH1, 1);
  Timer2.attachInterrupt(TIMER_CH1, handler_tim);
  
  Serial.begin(9600);           // start serial for output
}

int ports = B0000;
int temp_ports = B0000;
uint8_t column = 0;
char msg_buf[6] = {0, 0, 0, 0, 0, 0};



void loop()
{
  //Serial.println(buttonState);
  if (buttonState == 1){
    //Turn off I2C and reinitialize
    Wire.begin(I2C_ADDR); //join the i2c bus with a different address
    Wire.onRequest(requestEvent);
    Wire.onReceive(receiveEvent);
    Serial.print("Attempted Address Change");
    Serial.println(I2C_ADDR, HEX); 
    buttonState = 2;   
  }
  //Determine occupied directions
  temp_ports = B0000;
  if(digitalRead(PIN_DIR_U)){
    temp_ports = temp_ports | CNCT_U;
  }
  if(digitalRead(PIN_DIR_D)){
    temp_ports = temp_ports | CNCT_D;
  }
  if(digitalRead(PIN_DIR_L)){
    temp_ports = temp_ports | CNCT_L;
  }
  if(digitalRead(PIN_DIR_R)){
    temp_ports = temp_ports | CNCT_R;
  }
  ports = temp_ports;
  
  //Display 
  //x position

  if(dspy_en){
    pos_x = (int) msg_buf[0] - 48;
    pos_y = (int) msg_buf[1] - 48;
    brightness = (int) msg_buf[2] * 25;
    digitalWrite(led_out[pos_x][pos_y], HIGH);
  }

  
  delay(100);
  //Serial.println("continuing");
}

// function that executes whenever data is received from master
// this function is registered as an event, see setup()
void receiveEvent(int howMany)
{
  char c;
  Serial.println("Event Received");
  if(Wire.available()){
    c = Wire.read();      // receive first byte as a character
    Serial.println(c);         // print the character
  }
  if(c == 'A'){
    I2C_ADDR = Wire.read();    // receive byte as an integer
    Serial.println(I2C_ADDR, HEX);         // print the integer h
    if(buttonState != 2){
      Wire.end();
      buttonState = 1;
    }
  }else if (c == 'E'){
    dspy_en = 1;
    int i = 0;
    while(Wire.available()){
      msg_buf[i] = Wire.read();
      Serial.print(msg_buf[i]);
      i++;     
    }
    Serial.println();
  }else if (c == 'D'){
    dspy_en = 0;
  }
}

// function that executes whenever data is requested by master
// this function is registered as an event, see setup()
void requestEvent()
{
  Wire.write(ports);  // respond with message of 1 byte
                      // as expected by master
}
void handler_tim(void){
  digitalWrite(led_out[0][0], LOW);
  digitalWrite(led_out[1][0], LOW);
  digitalWrite(led_out[0][1], LOW);
  digitalWrite(led_out[1][1], LOW);
}
