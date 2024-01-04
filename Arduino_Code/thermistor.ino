float getTemperature(float *p_temperature, int *p_adc) {
  uint8_t i;
  float average;

  int samples[NUMSAMPLES];

  // take N samples in a row, with a slight delay
  for (i = 0; i < NUMSAMPLES; i++) {
    samples[i] = analogRead(THERMISTOR_PIN);
    delay(10);
  }

  // average all the samples out
  average = 0;
  for (i = 0; i < NUMSAMPLES; i++) {
    average += samples[i];
  }
  average /= NUMSAMPLES;

  //For printing
  *p_adc = average;

  // convert the value to resistance
  average = 1023 / average - 1;
  average = SERIESRESISTOR / average;

  float steinhart;
  steinhart = log(average / THERMISTORNOMINAL) / BCOEFFICIENT;  // (1/B) * ln(R/Ro)
  steinhart += 1.0 / (TEMPERATURENOMINAL + 273.15);             // + (1/To)
  steinhart = (1.0 / steinhart) - 273.15;                       // Invert and convert absolute temp to C                                        

  *p_temperature = steinhart;
}
