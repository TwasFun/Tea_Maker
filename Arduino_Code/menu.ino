void displayMenu() {

  u8x8.setFont(u8x8_font_px437wyse700b_2x2_r);

  u8x8.clear();
  u8x8.noInverse();
  u8x8.setCursor(0, 0);
  u8x8.print(F("Menu:"));
  u8x8.setCursor(0, 2);
  for (int i = topItem; i < min(topItem + visibleItems, itemCount); i++) {
    if (i == counter) {
      u8x8.inverse();
    } else {
      u8x8.noInverse();
    }
    u8x8.println(menuItems[i]);
  }

  u8x8.setFont(u8x8_font_amstrad_cpc_extended_f);
  u8x8.noInverse();
}

void brewTea(int current_tea, int setpoint) {
  beep();
  currentSetpoint = setpoint;

  unsigned long startTime = millis();
  unsigned long currentMillis = 0;
  unsigned long previousMillis = startTime;
  const unsigned long interval = SAMPlE_TIME * 1000;
  float x;
  int analog;
  getTemperature(&x, &analog);

  while (1) {
    if (readButton() == -1) {
      break;
    }
    currentMillis = millis() - startTime;
    if (currentMillis - previousMillis >= interval) {
      previousMillis = currentMillis;
      getTemperature(&x, &analog);

      if (current_tea != KEEP_WARM && x >= currentSetpoint) {
        currentSetpoint = menuTemperature[KEEP_WARM];
        current_tea = KEEP_WARM;
      } else if (current_tea == KEEP_WARM && ((x >= currentSetpoint - 4.0) && (x <= currentSetpoint))) {
        beep();
      }

      String timeString = String((currentMillis / 60000) % 60) + ":" + String((currentMillis / 1000) % 60);

      u8x8.clear();
      u8x8.noInverse();
      u8x8.setFont(u8x8_font_px437wyse700b_2x2_r);

      u8x8.setCursor(0, 0);
      u8x8.print(menuItems[current_tea]);
      u8x8.setCursor(12, 0);
      u8x8.print(currentSetpoint, 0);
      if (x >= 100) {
        u8x8.setCursor(7, 2);
      } else {
        u8x8.setCursor(8, 2);
      }
      u8x8.print(x, 1);

      int y = setPidOutput(&x);
      if (y <= 9) {
        u8x8.setCursor(12, 6);
      } else if (y >= 100) {
        u8x8.setCursor(8, 6);
      } else {
        u8x8.setCursor(10, 6);
      }


      u8x8.print(String(y) + "%");
      u8x8.setFont(u8x8_font_amstrad_cpc_extended_f);

      u8x8.setCursor(0, 2);
      u8x8.print(timeString);
      showPID(true);
    }
  }
}