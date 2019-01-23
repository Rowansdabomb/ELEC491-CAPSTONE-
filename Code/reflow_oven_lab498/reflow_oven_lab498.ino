#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <PID_v1.h>

double Setpoint, Input, Output;

double Kp=180, Ki=0.14, Kd=480;
PID ramp_1(&Input, &Output, &Setpoint, Kp, Ki, Kd, DIRECT);

double Kp=180, Ki=0.14, Kd=480;
PID ramp_2(&Input, &Output, &Setpoint, Kp, Ki, Kd, DIRECT);

int WindowSize = 1000;
unsigned long windowStartTime;
const int buttonPin = 2;     // the number of the pushbutton pin
const int buttonPin_2 = 3;
int raw0=0;
int raw1=0;
float Vin=5;
float v0=0;
float v1=0;
float buffer0=0;
float buffer1=0;

#define MOT 11
#define BOT 12
#define TOP 13

OneWire ds(2);

void setup()
{
  Serial.begin(9600);
  pinMode(MOT, OUTPUT);
  pinMode(BOT, OUTPUT);
  pinMode(TOP, OUTPUT);

  digitalWrite(MOT, HIGH);
  digitalWrite(BOT, LOW);
  digitalWrite(TOP, LOW);

  windowStartTime = millis();
  
  raw0= analogRead(buttonPin);
    buffer0=raw0*Vin;
      v0=buffer0/1024.0;
  raw1= analogRead(buttonPin_2);
    buffer1=raw1*Vin;
      v1=buffer1/1024.0;
      
  if ((v0 > v1) && (v0 > 3.0)) {
    Setpoint = 60;
    Serial.println("Set to reflow curve for T25 Capstone PCB");
  }
  else if ((v1 > v0) && (v1 > 3.0)) {
    Setpoint = 70;
    Serial.println("Setpoint is now: 70*C");
  }
  else {
    Setpoint = 0;
  Serial.println("OFF");
  }

  //tell the PID to range between 0 and the full window size
  ramp_1.SetOutputLimits(0, WindowSize);

  //tell the PID to range between 0 and the full window size
  ramp_2.SetOutputLimits(0, WindowSize);

  //turn the PID on
  ramp_1.SetMode(AUTOMATIC);

  //turn the PID off
//  ramp_2.SetMode(MANUAL);
   ramp_2.SetMode(AUTOMATIC);
}

int tempSet(currentTime, Output, celsius) {
    if (currentTime - windowStartTime > WindowSize) { //time to shift the Relay Window
      windowStartTime += WindowSize;
      //Serial.println("*********************************************************");
      Serial.println("adjusting time");
    }
    if (Output > currentTime - windowStartTime) {
      digitalWrite(TOP, HIGH);
      digitalWrite(BOT, HIGH);
      Serial.println("*********************************************************");
      Serial.println("ON");
      Serial.print("TEMP= "); Serial.print(celsius); Serial.println(" *C");
    }
    else {
     digitalWrite(TOP, LOW);
     digitalWrite(BOT, LOW);
     Serial.println("*********************************************************");
     Serial.println("OFF");
     Serial.print("TEMP= "); Serial.print(celsius); Serial.println(" *C");
   }
   return 0;
}

void loop()
{
  byte i;
  byte present = 0;
  byte type_s;
  byte data[12];
  byte addr[8];
  float celsius, fahrenheit;

  if ( !ds.search(addr)) {
    ds.reset_search();
    delay(250);
    return;
  }

  for ( i = 0; i < 8; i++) {
  }

  if (OneWire::crc8(addr, 7) != addr[7]) {
    Serial.println("CRC is not valid!");
    return;
  }

  // the first ROM byte indicates which chip
  switch (addr[0]) {
    case 0x10:
      type_s = 1;
      break;
    case 0x28:
      type_s = 0;
      break;
    case 0x22:
      type_s = 0;
      break;
    default:
      return;
  }

  ds.reset();
  ds.select(addr);
  ds.write(0x44, 1);        // start conversion, with parasite power on at the end

  //delay(1000);     // maybe 750ms is enough, maybe not
  // we might do a ds.depower() here, but the reset will take care of it.

  present = ds.reset();
  ds.select(addr);
  ds.write(0xBE);         // Read Scratchpad


  for ( i = 0; i < 9; i++) {           // we need 9 bytes
    data[i] = ds.read();
  }
  // Convert the data to actual temperature
  // because the result is a 16 bit signed integer, it should
  // be stored to an "int16_t" type, which is always 16 bits
  // even when compiled on a 32 bit processor.
  int16_t raw = (data[1] << 8) | data[0];
  if (type_s) {
    raw = raw << 3; // 9 bit resolution default
    if (data[7] == 0x10) {
      // "count remain" gives full 12 bit resolution
      raw = (raw & 0xFFF0) + 12 - data[6];
    }
  } else {
    byte cfg = (data[4] & 0x60);
    // at lower res, the low bits are undefined, so let's zero them
    if (cfg == 0x00) raw = raw & ~7;  // 9 bit resolution, 93.75 ms
    else if (cfg == 0x20) raw = raw & ~3; // 10 bit res, 187.5 ms
    else if (cfg == 0x40) raw = raw & ~1; // 11 bit res, 375 ms
    //// default is 12 bit resolution, 750 ms conversion time
  }
  
  celsius = (float)raw / 16.0;

  fahrenheit = celsius * 1.8 + 32.0;
  Input = celsius;
  
  
  unsigned long currentTime = millis();
  //first ramp
  if (celcius < FIRST_TEMP) {
    ramp_1.Compute();
    tempSet(currentTime, Output, celcius);
    state = RAMP_UP;
  }
  if (celsius >= FIRST_TEMP && state == FIRST_RAMP) {
    ramp_1.Compute();
    tempSet(currentTime, Output, celcius);
    ramp1FinishTime = currentTime;
    state = PLATEAU;
  }
  // plateau 1
  if (currentTime - ramp1FinishTime < FIRST_PLATEAU_TIME) {
    ramp1.Compute();
    tempSet(currentTime, Output, celcius);
    plateau1FinishTime = currentTime;
    state == PLATEAU;
  }
  if (currentTime - ramp1FinishTime >= FIRST_PLATEAU_TIME && celsius < SECOND_TEMP) {
    ramp2.Compute();
    tempSet(currentTime, Output, celcius);
    ramp2FinishTime
    state = RAMP_UP
  }
  if (currentTime - ramp_2_finishTime ) {
    ramp2.Compute();
    
  }
  if (state == COOL_DOWN) {
    
  }
  
  else if (currentTime - windowStartTime < SECOND_RAMP){
//    ramp_1.SetMode(MANUAL);
//    ramp_2.SetMode(AUTOMATIC);
    ramp_2.Compute();
  }
  while (Serial.available () > 0) {
    char c = Serial.read();
    if (c == 'm') {
      Serial.println("Turning motor on");
      digitalWrite(MOT, HIGH);
    }
    else if (c == 'n') {
      Serial.println("Turning motor off");
      digitalWrite(MOT, LOW);
    }
    else if (c == 'b') {
      Serial.println("Turning bottom on");
      digitalWrite(BOT, HIGH);
    }
    else if (c == 'v') {
      Serial.println("Turning bottom off");
      digitalWrite(BOT, LOW);
    }
    else if (c == 't') {
      Serial.println("Turning top on");
      digitalWrite(TOP, HIGH);
    }
    else if (c == 'r') {
      Serial.println("Turning top off");
      digitalWrite(TOP, LOW);
    }
    else if (c == 'e') {
      Serial.print("Dallas Temperature: "); Serial.print(celsius); Serial.println("*C");
    }
  }
}
