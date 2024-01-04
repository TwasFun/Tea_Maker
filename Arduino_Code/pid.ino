uint8_t initPID() {

  // Read the PID parameters from EEPROM
  EEPROM.get(EEPROM_PID_ADDRESS, pidParams);

  // Check if the PID parameters are valid
  bool pidParamsValid = true;
  for (int i = 0; i < 3; i++) {
    if (isnan(pidParams[i])) {
      DEBUG_PRINTLN("PID params not found");
      pidParamsValid = false;
      break;
    }
  }

  // Use the defaults if parameters are not valid
  if (!pidParamsValid) {
    DEBUG_PRINTLN("Using default PID params");
    for (int i = 0; i < 3; i++) {
      pidParams[i] = DEFAULT_PID[i];
    }
  }

  initPID2();
}

void initPID2() {
  // Set the PID controller parameters
  pid.SetTunings(pidParams[0], pidParams[1], pidParams[2]);

  pid.SetMode(AUTOMATIC);
  pid.SetOutputLimits(minHeaterPower, maxHeaterPower);
  pid.SetSampleTime(SAMPlE_TIME * 1000);  //Set to 60 seconds because our system is slow.
}

void savePID(double kp, double ki, double kd) {

  //Clear the EEPROM
  EEPROM.begin();
  for (int i = 0; i < 512; i++) {
    EEPROM.write(i, 0);
  }
  EEPROM.end();

  //Display message
  DEBUG_PRINT("EEPROM cleared...");

  // Save the PID parameters
  pidParams[0] = kp;
  pidParams[1] = ki;
  pidParams[2] = kd;

  EEPROM.put(EEPROM_PID_ADDRESS, pidParams);
}

void setOutput(uint8_t value) {
  pwmWrite(RELAY_PIN, mapToPWM(value));
  //analogWrite(RELAY_PIN, mapToPWM(value));
}

uint8_t setPidOutput(float *temperature) {
  // Update the input value for the PID controller
  input = *temperature;
  // Compute the output value using the PID controller
  pid.Compute();
  // Write the output value to the heating pad
  setOutput(output);
  return output;
}

void showPID(bool only_values) {
  u8x8.noInverse();

  for (int i = 0; i <= 2; i++) {
    u8x8.setCursor(0, i + 5);

    if (only_values) {
      u8x8.print(String(pidParams[i], DECIMAL_PLACES));
    } else {
      u8x8.print(pidItems[i] + String(pidParams[i], DECIMAL_PLACES));
    }
  }
}

void printPIDTest(float change) {

  const uint8_t num_items = sizeof(pidItems) / sizeof(pidItems[0]);
  const uint8_t variable_items = 3;

  u8x8.clear();
  u8x8.noInverse();
  u8x8.setCursor(0, 0);
  u8x8.print(F("Test:"));
  u8x8.setCursor(0, 2);
  for (int i = 0; i < num_items; i++) {
    if (i == sub_state) {
      u8x8.inverse();
      if (i == 0) {
        pidParams[i] = pidParams[i] + (change * kp_STEP);  //For kp keep the increments or decremets in 0.05
      } else if (i == 1) {
        pidParams[i] = pidParams[i] + (change * ki_STEP);  //For ki keep the increments or decremets in 0.01
      } else {
        pidParams[i] = pidParams[i] + (change * kd_STEP);  //For kd keep the increments or decremets in 1.00
      }

    } else {
      u8x8.noInverse();
    }
    u8x8.print(pidItems[i]);
    if (i < variable_items) {
      u8x8.print(pidParams[i], DECIMAL_PLACES);
    }
    u8x8.println();
  }
  u8x8.noInverse();
}