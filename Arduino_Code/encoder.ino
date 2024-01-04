void initEncoder() {
  // Read the initial state of CLK
  //lastStateCLK = digitalRead(CLK);
  attachInt();
}

void attachInt() {
  // Call updateEncoder() when any high/low changed seen
  // on interrupt 0 (pin 2), or interrupt 1 (pin 3)
  attachInterrupt(digitalPinToInterrupt(2), updateEncoder, CHANGE);
  attachInterrupt(digitalPinToInterrupt(3), updateEncoder, CHANGE);
}

void detachInt() {
  detachInterrupt(digitalPinToInterrupt(2));
  detachInterrupt(digitalPinToInterrupt(3));
}

int mapToPWM(int inputValue) {                // Function to map input (0-100) to PWM (0-255)
  inputValue = constrain(inputValue, 0, 100); // Ensure the input value is within the specified range
  return map(inputValue, 0, 100, 0, 255);    // Map the input value to the PWM range
}

void updateEncoder() {
  static uint8_t currentStateCLK;
  static uint8_t lastStateCLK;

  // Read the current state of CLK
  currentStateCLK = digitalRead(ENCODER_CLK);

  prevCounter = counter;

  // If last and current state of CLK are different, then pulse occurred
  // React to only 1 state change to avoid double count
  if (currentStateCLK != lastStateCLK /*&& currentStateCLK == 1*/) {

    if (digitalRead(ENCODER_DT) != currentStateCLK) {
      // Encoder is rotating CW so increment
      counter++;
    } else {
      counter--;
    }
    updateLCD = true;
  }

  // Remember last CLK state
  lastStateCLK = currentStateCLK;
}
