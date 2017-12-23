#include <Wire.h>
#include "defs-ino.h" // This should eventually be the shared defs.h

void setup() {
	Serial.begin(9600);
	Wire.begin();
}

u8 setLed(u8 index, u32 color) {
		Wire.beginTransmission(TWI_BASE_ADDRESS);
		Wire.write(TWI_CMD_SET_LED);
		Wire.write(index);
		Wire.write((u8 *)&color, sizeof(color));
		return Wire.endTransmission();
}

u8 setAllLeds(u32 color) {
		Wire.beginTransmission(TWI_BASE_ADDRESS);
		Wire.write(TWI_CMD_SET_ALL_LEDS);
		Wire.write((u8 *)&color, sizeof(color));
		return Wire.endTransmission();
}

u8 setLedPattern(u32 *colors) {
		Wire.beginTransmission(TWI_BASE_ADDRESS);
		Wire.write(TWI_CMD_SET_LED_PATTERN);
		for (u8 i = 0; i < LED_COUNT; ++i) {
			Wire.write((u8 *)(colors + i), sizeof(colors[0]));
		}
		return Wire.endTransmission();
}

void nextLed() {
	static u8 index = 0;

/*  if (index < 1) {
		u8 result = setAllLeds(0x1333333);
		Serial.print(result);
	}
	else if (index < 11) {
		u8 result = setLed(index - 1, 0x1FF3333);
		Serial.print(result);
	}
	else if (index < 21) {
		u8 result = setLed(index - 11, 0x133FF33);
		Serial.print(result);
	}
	else if (index < 31) {
		u8 result = setLed(index - 21, 0x13333FF);
		Serial.print(result);
	}
	else {
*/
		if (index % 2 == 0) {
			u32 colors[LED_COUNT] = {
				0x1FF0000, 0x100FF00, 0x1FF0000, 0x100FF00, 0x1FF0000, 0x100FF00, 0x1FF0000, 0x100FF00, 0x1FF0000, 0x100FF00
			};
			u8 result = setLedPattern(colors);
			Serial.print(result);
		} else {
			u32 colors[LED_COUNT] = {
				0x100FF00, 0x1FF0000, 0x100FF00, 0x1FF0000, 0x100FF00, 0x1FF0000, 0x100FF00, 0x1FF0000, 0x100FF00, 0x1FF0000
			};
			u8 result = setLedPattern(colors);
			Serial.print(result);
		}
/*
	}
*/

	index = (index + 1) % 2;
}

void loop() {
	nextLed();
	delay(500);
}
