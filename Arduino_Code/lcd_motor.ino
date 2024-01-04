void initDisplay() {
  u8x8.begin();
  //u8x8.setFlipMode(1);
  //u8x8.setFont(u8x8_font_amstrad_cpc_extended_f);
  //u8x8.setFont(u8x8_font_chroma48medium8_r);
  //u8x8.setFont(u8x8_font_px437wyse700b_2x2_r);
  //u8x8.setFont(u8x8_font_inb33_3x6_n);
  //u8x8.setFont(u8x8_font_pxplusibmcgathin_f);

  u8x8.clear();
  u8x8.setFont(u8x8_font_amstrad_cpc_extended_f);
}

void mix() {
  uint8_t factor = 2;
  counter = constrain(counter, 0, 100/factor);
  analogWrite(MOTOR_PIN, mapToPWM(counter * factor));
  u8x8.clear();
  u8x8.setCursor(0, 0);
  u8x8.print("Speed: ");
  u8x8.print(counter * factor);
  u8x8.println(" %");
}