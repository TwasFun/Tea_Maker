int readButton() {

  //For Handling Button
  static unsigned long pressedTime = 0;
  static unsigned long releasedTime = 0;

  button.loop();  // MUST call the loop() function first

  if (button.isPressed()) {
    pressedTime = millis();
    //LONG_PRESS_TIME
  }

  if (button.isReleased()) {
    releasedTime = millis();

    long pressDuration = releasedTime - pressedTime;
    //SHORT_PRESS_TIME
    if (pressDuration < 500) {

        if(STATE == STATE_BREW || STATE == STATE_MIX){            //Return if we are alrady brewing or we are changing motor values
          return 0;
        }

        int x = menuTemperature[counter];

        if (STATE == STATE_IDLE && (counter >=0 && counter <=2)) {  //Only execute when in (Main Menu and counter is between first three tea options)
          detachInt();
          STATE = STATE_BREW;
          brewTea(counter, x);
        } else if (STATE == TEST_PID || x == TEST_PID ) {           //Only execute when (already changing PID values or first time selecting test PID option)
          STATE = TEST_PID;
          updateLCD = true;
          prevCounter = 0;
          counter = 0;
          sub_state++;
          if (sub_state > (sizeof(pidItems) / sizeof(pidItems[0]))-1) {
            sub_state = 0;
          }
        } else if(STATE == STATE_IDLE) {                          //Only execute when in Main Menu
          switch (x) {

            case SHOW_PID:
              STATE = SHOW_PID;
              u8x8.clear();
              showPID(false);
              break;

            case STATE_MIX:
              STATE = STATE_MIX;
              counter = 0;
              mix();
              break;

            default:
              u8x8.clear();
              u8x8.setCursor(0, 0);
              u8x8.print(F("error"));
              delay(500);
              break;
          }
        }
        return 0;
    }

    if (pressedTime > 1500) {
      STATE = STATE_IDLE;
      u8x8.clear();
      u8x8.setCursor(0, 0);
      u8x8.println(F("Cancelled..."));
      delay(1500);
      topItem = 0;
      counter = 1;
      setOutput(minHeaterPower);
      attachInt();
      displayMenu();
      return -1;
    }
  }
  return 0;
}