#include <Wire.h>   //for I2C devices
#include "U8glib.h"  //for display

#define LEFT 0
#define SELECT 1
#define RIGHT 2

U8GLIB_SSD1306_ADAFRUIT_128X64 u8g(10, 9); //start display with serial SPI

boolean midiMode = true;  //choose midi or human-readable output
boolean hairless = true;   //if sending midi, USB format or hairless?
const byte minBoard = 0;
const byte maxBoard = 6;

boolean noteStates[128] = {0};
boolean enabled[7] = {1, 1, 1, 1, 1, 1, 1};
boolean calibrating[7] = {0, 0, 0, 0, 0, 0, 0};

byte buttonPins[] = {2, 3, 4};
byte buttonStates[] = {0, 0, 0};
byte sustainPin = A3;
byte sustainState = 0;

void setup() {
  Wire.begin();        // join i2c bus (address optional for master)
  Wire.setClock(400000L);  //400kHz speed

  pinMode(8, OUTPUT); //OLED reset pin
  digitalWrite(8, LOW); //reset the OLED
  delay(10);
  digitalWrite(8, HIGH);
  u8g.setColorIndex(1);  //set mode to BW
  u8g.begin();

  Serial.begin(115200);  // start serial for output
  //Serial.println("Piano2Midi Controller V0.1");
  for (int i = 0; i < 3; i++) {
    pinMode(buttonPins[i], INPUT_PULLUP);
  }
  pinMode(sustainPin, INPUT_PULLUP);
}

void loop() {
  buttonUpdate();       //updates button states
  buttonActions();      //updates what they do
  updateDisplay();      //updates the display
  getMessages();        //checks for any MIDI notes over I2C, and sends them as midi messages
  updateSustain(); //check whether sustain is on/off and send midi accordingly. Audio jack: tip-A3, mid-5V, collar-GND
}


//compatible with USB MIDI firmware
void midiMessage(int commandByte, int channelByte, int data1Byte, int data2Byte) {
  if (midiMode) {
    Serial.write(commandByte);
    if (!hairless) {
      Serial.write(channelByte);
    }
    Serial.write(data1Byte);
    Serial.write(data2Byte);
  } else {
    Serial.print(commandByte); Serial.print("\t");
    Serial.print(data1Byte); Serial.print("\t");
    Serial.println(data2Byte);
  }
}

void beginCalibration(byte BOARD) {
  Wire.beginTransmission(BOARD); // transmit to device #BOARD
  Wire.write('c');               //send 'c' char to begin calibration on selected board
  Wire.endTransmission();    // stop transmitting
}
void endCalibration(byte BOARD) {
  Wire.beginTransmission(BOARD); // transmit to device #BOARD
  Wire.write('e');               //send 'c' char to begin calibration on selected board
  Wire.endTransmission();    // stop transmitting
}

