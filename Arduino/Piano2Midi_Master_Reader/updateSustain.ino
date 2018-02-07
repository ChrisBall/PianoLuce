void updateSustain() {
  //read analog A3 - should be a digital switch (microswitch)
  boolean state = digitalRead(sustainPin);
  if (sustainState == 3) {
    sustainState = 2;
  }
  if (!state && (sustainState == 0)) {
    sustainState = 3;
  }
  if (sustainState == 1) {
    sustainState = 0;
  }
  if (state && (sustainState == 2)) {
    sustainState = 1;
  }

  if(sustainState==3){
    midiMessage(0xB0, 1, 64, 127);
  }else if(sustainState==1){
    midiMessage(0xB0, 1, 64,0);
  }
}
