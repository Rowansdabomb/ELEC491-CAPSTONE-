//#include "TimerOne.h"

#define SENSOR_POLL_PERIOD 1000

const uint8_t sensorDataSize = 16;
volatile int sensorData[sensorDataSize] = {0};

const uint8_t led = PC13;

const uint8_t matrixWidth = 4;
const uint8_t matrixHeight = 4;

const uint8_t timerInterval = 100;
volatile bool interruptFlag = false;
volatile unsigned long interruptTime = 0;

HardwareTimer timer(2);

void setup() {
  // put your setup code here, to run once:
  pinMode(led, OUTPUT);
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
  unsigned long temp = micros(); 
  uint8_t si = 0;
  for(uint8_t i = 0; i < 10; i++) {
    if(i < 8)
      sensorData[(sensorDataSize - 1) - (i+1)] = analogRead(i);
    else
      sensorData[(sensorDataSize - 1) - (i+1)] = analogRead(i + 10);
  }
  interruptTime = micros() - temp;
  interruptFlag = true;
}

void printSensorData() {
  Serial.flush();
  for (uint8_t i = 0; i < sensorDataSize; i++) {
    Serial.print(i);
    Serial.print(": ");
    Serial.print(sensorData[i]);
    Serial.print(", ");
  }
  Serial.println();
  
  for (uint8_t i = 0; i < matrixHeight; i++) {
    for(uint8_t j = 0; j < matrixWidth; j++) {
      if(j < matrixWidth - 1) {
        Serial.print(sensorData[i + j*(matrixHeight - 1)]);
        Serial.print(" | ");
      }
      else
        Serial.println(sensorData[i+j*(matrixHeight - 1)]);
    }
  }
  Serial.println(interruptTime);
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
