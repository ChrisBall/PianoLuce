void printSensorData() {
//  for (int i = 0; i < 12; i++) {
//    Serial.print(sensorAnalogReadings[i]);
//    Serial.print("\t");
//  }
//  Serial.println();
  byte sensor=6;
  Serial.print(sensorReading[sensor]);
  Serial.print("\t");
  Serial.print(minSensorReading[sensor]);
  Serial.print("\t");
  Serial.print(maxSensorReading[sensor]);
  Serial.print("\t");
  Serial.print(uThreshold[sensor]);
  Serial.print("\t");
  Serial.print(lThreshold[sensor]);
  Serial.print("\t");
  Serial.print(noteTimer[sensor]);
  Serial.print("\t");
  
  if (!midiBuffer.isEmpty()) {
    midiMessage sneaky=midiBuffer.peek();
    Serial.print(midiBuffer.count());
  }
  
  Serial.println();
}

