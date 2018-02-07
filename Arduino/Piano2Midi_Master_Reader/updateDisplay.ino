unsigned int updateFrequency = 256;
long unsigned int count = 0;

//which board is selected in the menu
char boardSelected = 4;
byte subMenuSelected = 0;
boolean subMenu = false;

void updateDisplay() {
  if (count % updateFrequency == 0) {
    u8g.firstPage();
    do {
      draw();
    } while ( u8g.nextPage() );
  }
  count++;
}

void draw(void) {
  //draw title
  u8g.setFont(u8g_font_4x6);
  u8g.setPrintPos(0, 6);
  u8g.print("Piano2Midi Controller V0.1");

  //draw note states
  for (int i = 0; i < 128; i++) {
    if (noteStates[i]) {
      u8g.drawLine(i, 10, i, 20);
    }
  }

  //draw board indicators
  u8g.setFont(u8g_font_7x14B);
  for (int i = minBoard; i < maxBoard + 1; i++) {
    u8g.setPrintPos(16 + i * 16, 37);
    u8g.print(i);
  }

  //draw highlighted board
  u8g.drawFrame(14 + boardSelected * 16, 25, 10, 14);

  //draw sub-menu
  if (subMenu) {
    u8g.setFont(u8g_font_5x8);
    u8g.setPrintPos(5, 48);
    u8g.print("back  ");

    if (!calibrating[boardSelected]) {
      u8g.print("calibrate  ");
    } else {
      u8g.print("end        ");
    }

    if (enabled[boardSelected]) {
      u8g.print("disable");
    } else {
      u8g.print("enable");
    }
    if (subMenuSelected == 0) {
      u8g.drawFrame(1, 40, 25, 10);
    } else if (subMenuSelected == 1) {
      u8g.drawFrame(34, 40, 47, 10);
    } else {
      u8g.drawFrame(88, 40, 38, 10);
    }
  }

  //draw sustain indicator
  u8g.setPrintPos(5, 63);
  if(sustainState>>1){
    u8g.print("Sustain on");
  }else{
    u8g.print("Sustain off");
  }

}

void buttonActions() {
  if (subMenu) {
    if (buttonStates[LEFT] == 3) {
      subMenuSelected--;
      if (subMenuSelected > 2) {
        subMenuSelected = 2;
      }
    }

    if (buttonStates[SELECT] == 3) {

      if (subMenuSelected == 0) {
        subMenu = false;
      } else if (subMenuSelected == 1) {
        if (!calibrating[boardSelected]) {
          beginCalibration(boardSelected);
          calibrating[boardSelected] = true;
        }else{
          endCalibration(boardSelected);
          calibrating[boardSelected] = false;
        }
      } else {
        enabled[boardSelected] = !enabled[boardSelected];
      }
    }

    if (buttonStates[RIGHT] == 3) {
      subMenuSelected++;
      subMenuSelected %= 3;
    }

  } else {

    if (buttonStates[LEFT] == 3) {
      boardSelected--;
      if (boardSelected < minBoard) {
        boardSelected = maxBoard;
      }
    }

    if (buttonStates[SELECT] == 3) {
      subMenu = true;
      subMenuSelected = 0;
    }

    if (buttonStates[RIGHT] == 3) {
      boardSelected++;
      if (boardSelected > maxBoard) {
        boardSelected = minBoard;
      }
    }
  }
}


