#include <Wire.h>
#include "defs-ino.h" // This should eventually be the shared defs.h

void setup() {
	Serial.begin(9600);
	Wire.begin();
}

void setLed(u8 index, u32 color) {
	Wire.beginTransmission(TWI_BASE_ADDRESS);
	Wire.write(TWI_CMD_SET_LED);
	Wire.write(index);
	Wire.write((u8 *)&color, sizeof(color));
	Serial.print(Wire.endTransmission());
}

void setAllLeds(u32 color) {
	Wire.beginTransmission(TWI_BASE_ADDRESS);
	Wire.write(TWI_CMD_SET_ALL_LEDS);
	Wire.write((u8 *)&color, sizeof(color));
	Serial.print(Wire.endTransmission());
}

void setLedPattern(u32 *colors) {
	for (u8 bank = 0; bank < LED_BANKS; ++bank) {
		Wire.beginTransmission(TWI_BASE_ADDRESS);
		Wire.write(TWI_CMD_SET_LED_BANK);
		Wire.write(bank == LED_BANKS - 1);
		Wire.write(bank);
		for (u8 index = bank * LED_BANK_SIZE;
			index < (bank + 1) * LED_BANK_SIZE && index < LED_COUNT;
			++index
		) {
			Wire.write((u8 *)(colors + index), sizeof(colors[0]));
		}
		Serial.print(Wire.endTransmission());
		delay(1);
	}
}

void nextLed() {
	static u8 index = 0;

	if (index < 1) {
		setAllLeds(0x1000000);
	}
	else if (index < 11) {
		setLed(index - 1, 0x1FF0000);
	}
	else if (index < 21) {
		setLed(index - 11, 0x100FF00);
	}
	else if (index < 31) {
		setLed(index - 21, 0x1000000);
	}
	else {
		if (index % 2) {
			u32 colors[LED_COUNT] = {
				0x1FF0000, 0x100FF00, 0x1FF0000, 0x100FF00, 0x1FF0000, 0x100FF00, 0x1FF0000, 0x100FF00, 0x1FF0000, 0x100FF00
			};
			setLedPattern(colors);
		} else {
			u32 colors[LED_COUNT] = {
				0x100FF00, 0x1FF0000, 0x100FF00, 0x1FF0000, 0x100FF00, 0x1FF0000, 0x100FF00, 0x1FF0000, 0x100FF00, 0x1FF0000
			};
			setLedPattern(colors);
		}
	}

	index = (index + 1) % 48;
}

void loop() {
	nextLed();
	delay(100);
}
