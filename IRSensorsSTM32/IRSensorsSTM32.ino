//#include "TimerOne.h"

#define SENSOR_POLL_PERIOD 100

const uint8_t sensorDataSize = 16;
volatile int sensorData[sensorDataSize] = {0};

const uint8_t led = PC13;
const uint8_t testPin = PB12;

const uint8_t matrixWidth = 4;
const uint8_t matrixHeight = 4;

volatile bool interruptFlag = false;
volatile unsigned long interruptTime = 0;

HardwareTimer timer(2);

void setup() {
  // put your setup code here, to run once:
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
      sensorData[i] = analogRead(pin)/16; //convert to 255
//      sensorData[i] = pin;
    }
    else {
      sensorData[i] = analogRead(pin + 10)/16;
//      sensorData[i] = pin + 10;  
    }
  }
  gpio_write_bit(GPIOB, 12, LOW);
  interruptFlag = true;
}

void printSensorData() {
  Serial.write(27);       // ESC command
  Serial.print("[2J");    // clear screen command
  Serial.write(27);
  Serial.print("[H");     // cursor to home command

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

//Simply a while loop that gets the sensor poll data and prints it on update
void loop() {
  // put your main code here, to run repeatedly:

  Serial.println("Let's begin!");
  while(true){
//    digitalWrite(led, HIGH);
//    delay(200);
//    digitalWrite(led, LOW);
//    delay(200);
    
    if (interruptFlag){
      if (digitalRead(led) == HIGH) {
        digitalWrite(led, LOW);
      } else {
        digitalWrite(led, HIGH);
      }
      
      //print the sensor data 
      printSensorData();
      
      //reset flag
      interruptFlag = false;
    }
  }
}
