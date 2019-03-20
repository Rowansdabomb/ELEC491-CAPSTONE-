#define SENSOR_POLL_PERIOD 50 // milliseconds

const uint8_t sensorDataSize = 64;

// Pin setup
const uint8_t led = PC13;
const uint8_t testPin = PB12;

//TODO
// row mux pin maps
const char MUX_ROW_SELECT[3] = {0, 1, 2};
//col muc pin maps
const char MUX_COL_SELECT[3] = {0, 1, 2};

const uint8_t MATRIX_WIDTH = 8;
const uint8_t MATRIX_HEIGHT = 8;

// Interrupt variables
volatile char sensorRow = 0; // 0-7
volatile char sensorCol = 0; // 0-7

volatile int sensorData[sensorDataSize] = {0};
// Sensors Read Interrupt Flag
volatile bool SRIF = false;
int bigArray[1000] = {0};

HardwareTimer timer(2);

void setup() {
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

// updates to the next sensor
void sensorRead() {
  gpio_write_bit(GPIOB, 12, HIGH); // time test
  
  // read sensor data, pin map should be 0-7 for A0-A7 so we use sensorCol
  sensorData[sensorCol + sensorRow*8] = analogRead(sensorCol);

   // turn on next emitter
  ++sensorRow;
  if (sensorRow > 7) sensorRow = 0;
  ++sensorCol;
  if (sensorCol > 7) sensorCol = 0;
  for (int i = 0; i < 3; ++i) {
    if ((sensorRow >> i) & 1) gpio_write_bit(GPIOB, MUX_ROW_SELECT[i], LOW);
    else gpio_write_bit(GPIOB, MUX_ROW_SELECT[i], HIGH);
    if ((sensorCol >> i) & 1) gpio_write_bit(GPIOB, MUX_COL_SELECT[i], LOW);
    else gpio_write_bit(GPIOB, MUX_COL_SELECT[i], HIGH);
  }
  if (sensorRow == 7 && sensorCol == 7) SRIF = true;

  gpio_write_bit(GPIOB, 12, LOW); // time test
}

void printSensorData() {
  Serial.write(27);       // ESC command
  Serial.print("[2J");    // clear screen command
  Serial.write(27);
  Serial.print("[H");     // cursor to home command

  for (uint8_t i = 0; i < MATRIX_WIDTH; ++i) {
    for(uint8_t j = 0; j < MATRIX_HEIGHT; ++j) {
      Serial.print(sensorData[i + j*(MATRIX_HEIGHT)]);
      if(j < MATRIX_HEIGHT - 1)
        Serial.print(" | ");
      else
        Serial.println();
    }
  }
  Serial.println();
}

void loop() {

  for (int i = 0; i < sizeof(bigArray)/sizeof(int); ++i) {
    bigArray[i] = i;
  }
  // put your main code here, to run repeatedly:
  while(true){

    // handle fresh sensor data
    if (SRIF){      
      //reset flag
      SRIF = false;
      
      //print the sensor data 
      printSensorData();
    }
  }
}
