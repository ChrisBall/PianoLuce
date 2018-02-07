
/*function that executes every 1ms (?TBC?) to read sensor data.
   we do not switch on the LED (TVCC) on black keys, instead relying on reflected light from the white keys/ambient light (this is a bad idea).
   This inverts the response on the black keys, which will be low->high, instead of high->low.
*/

void pollSensors() {
  for (int i = 0; i < 12; i++) {
    byte pin = pollOrder[i];

    //set multiplexer bits
    if (whiteKey[pin]) {
      PORTD = (muxPins[pin] << 2) + 64; //this line writes the multiplexer bits (muxPins[pin]<<2) and the TVCC bit (+64)
    } else {
      PORTD = (muxPins[pin] << 2);   //we don't write the TVCC bit for black keys - we leave the output IR LED off
    }

    //wait for LED to rise (and previous to fall) - this is not perfect and there is always residual light.
    delayMicroseconds(40);

    //get reading
    sensorReading[pin] = analogRead(A0);

    //work out new thresholds if we're calibrating
    if (calibrateSensors) {
      minSensorReading[pin] = min(minSensorReading[pin], sensorReading[pin]);
      maxSensorReading[pin] = max(maxSensorReading[pin], sensorReading[pin]);
      uThreshold[pin] = minSensorReading[pin] + (((maxSensorReading[pin] - minSensorReading[pin]) * 5) >> 3);
      lThreshold[pin] = minSensorReading[pin] + (((maxSensorReading[pin] - minSensorReading[pin]) * 3) >> 3);
    }

    if ((sensorReading[pin] <= uThreshold[pin]) && (sensorReading[pin] >= lThreshold[pin]) ) { //if between thresholds, add to the timer
      if (noteTimer[pin] < 99) {
        noteTimer[pin]++;
      }
    }

    //basic triggering; note On:
    if ((sensorReading[pin] < lThreshold[pin]) && (!noteStates[pin])) {
      noteStates[pin] = true;
      midiMessage noteEvent;
      if (whiteKey[pin]) {
        sendMessage(0x90, midiNote[pin], velocityLookup[noteTimer[pin]]); //Note on
      } else {
        sendMessage(0x80, midiNote[pin], 100); //Note off
      }
      noteTimer[pin] = 0;
    }

    //basic triggering; note Off:
    if ((sensorReading[pin] > uThreshold[pin]) && (noteStates[pin])) {
      noteStates[pin] = false;
      midiMessage noteEvent;
      if (whiteKey[pin]) {
        sendMessage(0x80, midiNote[pin], 100); //Note off
      } else {
        noteTimer[pin] = constrain(noteTimer[pin] * 2, 0, 99);
        sendMessage(0x90, midiNote[pin], velocityLookup[noteTimer[pin]]); //Note on
      }
      noteTimer[pin] = 0;
    }

    if (sensorReading[pin] < lThreshold[pin]) {
      noteTimer[pin] = 0;
    }
  }
}


void sendMessage(byte type, byte val1, byte val2) {
  midiMessage noteEvent;
  noteEvent.type = type;
  noteEvent.val1 = val1;
  noteEvent.val2 = val2;
  if (midiBuffer.count() < MIDIBUFFERSIZE) {
    midiBuffer.push(noteEvent);
  }
}


