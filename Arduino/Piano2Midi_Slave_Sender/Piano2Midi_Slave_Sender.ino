//measures analog inputs and stores any triggers as 3 byte messages in FIFO array
//initially triggers are on/off, no velocity
//returns midi messages upon request from master

#include <QueueArray.h>  //for managing midi FIFO buffer
#include <Wire.h>        //for I2C
#include <TimerOne.h>    //for managing interrupt
#include <EEPROM.h>      //for storing calibration data


//CONFIG
#define ADDRESS 0
/* addresses are the octave the sensor is responsible for; 0=lowest, 8=highest.
   We are actually using 7 boards from C1 to B7 (we won't bother with A0,A#0,B0 & C8)
   1 = C1 to B1  = MIDI notes 24 to 35
   2 = C2 to B2  = MIDI notes 36 to 47
   3 = C3 to B3  = MIDI notes 48 to 59
   4 = C4 to B4  = MIDI notes 60 to 71
   5 = C5 to B5  = MIDI notes 72 to 83
   6 = C6 to B6  = MIDI notes 84 to 95
   7 = C7 to B7  = MIDI notes 96 to 107
*/

#define MIDIBUFFERSIZE 24
/* this is the size of the MIDI message buffer, where midi messages are temporarily
   stored until sending over I2C at an appropriate time. Default = TBC
*/

#define INTERRUPTPERIOD 1000 //1 milliseconds ~ 1000Hz. A 'fast' piano keypress is estimated to take 2.5ms
//see curves here: https://focusritedevelopmentteam.wordpress.com/2012/10/04/or-it-is-the-sound/

//END CONFIG

#define A 2    
#define B 3
#define C 4
#define D 5
#define TVCC 6    

// defines for setting and clearing register bits (for faster ADC)
#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif
#ifndef sbi
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif

const byte velocityLookup[100] = {
  127, 127, 127, 127, 108, 97, 89, 83, 78, 73,
  70, 66, 64, 61, 58, 56, 54, 52, 51, 49,
  47, 46, 44, 43, 42, 41, 39, 38, 37, 36,
  35, 34, 33, 32, 31, 31, 30, 29, 28, 27,
  27, 26, 25, 25, 24, 23, 23, 22, 21, 21,
  20, 20, 19, 19, 18, 18, 17, 16, 16, 16,
  15, 15, 14, 14, 13, 13, 12, 12, 11, 11,
  11, 10, 10, 9, 9, 9, 8, 8, 8, 7,
  7, 7, 6, 6, 5, 5, 5, 4, 4, 4,
  4, 3, 3, 3, 2, 2, 2, 1, 1
};
const byte muxPins[] = {15, 14, 13, 12, 11, 10, 5, 4, 3, 2, 1, 0}; //this will change on future boards
const byte pollOrder[] = {0, 6, 1, 7, 2, 8, 3, 9, 4, 10, 5, 11};
const boolean whiteKey[] = {1, 0, 1, 0, 1, 1, 0, 1, 0, 1, 0, 1};
const byte midiNote[] = {
  (ADDRESS + 2) * 12 + 0,
  (ADDRESS + 2) * 12 + 1,
  (ADDRESS + 2) * 12 + 2,
  (ADDRESS + 2) * 12 + 3,
  (ADDRESS + 2) * 12 + 4,
  (ADDRESS + 2) * 12 + 5,
  (ADDRESS + 2) * 12 + 6,
  (ADDRESS + 2) * 12 + 7,
  (ADDRESS + 2) * 12 + 8,
  (ADDRESS + 2) * 12 + 9,
  (ADDRESS + 2) * 12 + 10,
  (ADDRESS + 2) * 12 + 11
};

int sensorReading[12] = {0};

//used during calibration
boolean calibrateSensors = false;
int maxSensorReading[12] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
int minSensorReading[12] = {1023, 1023, 1023, 1023, 1023, 1023, 1023, 1023, 1023, 1023, 1023, 1023};
int uThreshold[12] = {511, 511, 511, 511, 511, 511, 511, 511, 511, 511, 511, 511};
int lThreshold[12] = {511, 511, 511, 511, 511, 511, 511, 511, 511, 511, 511, 511};
byte noteTimer[12] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

boolean noteStates[12] = {0};

struct midiMessage {
  byte type;
  byte val1;
  byte val2;
};

QueueArray <midiMessage> midiBuffer;

void setup() {
  delay(50 + ADDRESS * 50);                 // wait for master and lower numbered slaves to become established, then...
  Wire.begin(ADDRESS);                      // join i2c bus with chosen address
  Wire.setClock(400000L);                   // comment if failure at high rates
  Wire.onRequest(requestEvent);             // register event
  Wire.onReceive(receiveEvent);             // register event (for calibration)
  Timer1.initialize(INTERRUPTPERIOD);       //delay time between sensor sweeps (1ms)
  Timer1.attachInterrupt(pollSensors);
  Serial.begin(115200);                     //serial for debugging
  //set up pins
  for (int i = 2; i < 7; i++) {
    pinMode(i, OUTPUT);  //mux A,B,C,D and TVCC (voltage supply for the output LED mux)
  }
  digitalWrite(TVCC, LOW);
  loadCalibration();  //loads calibration data from EEPROM

  // set prescale to 8 (9uS per analogRead)
  // cbi(ADCSRA,ADPS2) ;
  // sbi(ADCSRA,ADPS1) ;
  // sbi(ADCSRA,ADPS0) ;
  // set prescale to 16 (16uS per analogRead)
  sbi(ADCSRA, ADPS2) ;
  cbi(ADCSRA, ADPS1) ;
  cbi(ADCSRA, ADPS0) ;
}

void loop() {
  if (Serial) {
    printSensorData();
  }
}

// function that executes whenever data is requested by master
// this function is registered as an event, see setup()
void requestEvent() {
  if (!midiBuffer.isEmpty()) {
    midiMessage MM = midiBuffer.dequeue();  //is this unnecessary?
    byte midiOut[] = {MM.type, MM.val1, MM.val2};                     //can reduce this to two bytes. one has the note value 0-127 and the on/off bit, the other has the velocity
    Wire.write(midiOut, 3);
  } else {
    Wire.write(0); //meaning we have no data
    Wire.write(0);
    Wire.write(0);
  }
}

void receiveEvent(int howMany) {
  while (Wire.available()) { // loop through all but the last
    char type = Wire.read(); // receive byte as a character
    if (type == 'c') {    //"calibrate"
      for (int i = 0; i < 12; i++) {
        maxSensorReading[i] = 0;
        minSensorReading[i] = 1023;
        uThreshold[i] = 511;
        lThreshold[i] = 511;
      }
      calibrateSensors = true;
    } else if (type == 'e') {  //"end calibrate"
      calibrateSensors = false;
      saveCalibration();  //save calibration data to EEPROM
    }
  }
}

void loadCalibration() {
  noInterrupts();
  //load calibration data from eeprom
  for (int i = 0; i < 12; i++) {
    lThreshold[i] = EEPROM.read(i) << 2;      //lower thresholds stored in EEPROM addresses 0-11
  }
  for (int i = 0; i < 12; i++) {
    uThreshold[i] = EEPROM.read(i + 12) << 2; //upper thresholds stored in EEPROM addresses 12-23
  }
  interrupts();
}

void saveCalibration() {
  noInterrupts();
  //save calibration data to EEPROM - we lose a couple of bits accuracy in the process
  for (int i = 0; i < 12; i++) {
    EEPROM.write(i, lThreshold[i] >> 2);      //lower thresholds stored in EEPROM addresses 0-11
  }
  for (int i = 0; i < 12; i++) {
    EEPROM.write(i + 12, uThreshold[i] >> 2); //upper thresholds stored in EEPROM addresses 12-23
  }
  interrupts();
}


