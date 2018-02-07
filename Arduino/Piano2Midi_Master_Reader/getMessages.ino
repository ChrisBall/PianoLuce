void getMessages() {
  byte message[3]={0,0,0};
  for (int address = minBoard; address <= maxBoard; address++) {
    if (enabled[address]) {
      boolean dataAvailable = true;
      while (dataAvailable) {
        Wire.requestFrom(address, 3);    // request 3 bytes (noteOn/Off, note, velocity) from slave device address

        if (Wire.available()) { // slave can send less than requested
          message[0] = Wire.read();
          message[1] = Wire.read();
          message[2] = Wire.read();
        }

        if (!((message[0] == 0) && (message[1] == 0) && (message[2] == 0))) {
          //update records
          if (message[0] == 0x90) {
            noteStates[message[1]] = true;
            midiMessage(message[0], 1, message[1], message[2]);

          } else if (message[0] == 0x80) {
            noteStates[message[1]] = false;
            midiMessage(message[0], 1, message[1], message[2]);
          }
        } else {
          dataAvailable = false;
        }
      }
    }
  }
}
