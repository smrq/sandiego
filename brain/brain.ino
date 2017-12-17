#include <Wire.h>

void setup() {
  Wire.begin();
}

void loop() {
  static uint8_t ledIndex = 0;
  static uint8_t colorIndex = 0;

  uint32_t color;
  switch (colorIndex) {
    case 0:
      color = 0x04FF0000;      
      break;
    case 1:
      color = 0x0400FF00;
      break;
    case 2:
      color = 0x040000FF;
      break;
  }
  
  Wire.beginTransmission(0x58);
  Wire.write(0x73);
  Wire.write(ledIndex);
  Wire.write((uint8_t *)&color, sizeof(color));
  Wire.endTransmission();

  ledIndex = (ledIndex + 1) % 10;
  colorIndex = (colorIndex + 1) % 3;

  delay(1000);
}

