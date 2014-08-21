#include "Arduino.h"

#define REPORT_SIZE  16
#define REPORT_BYTES (REPORT_SIZE-1)

typedef struct {
	uint8_t modifiers;
	uint8_t keys[REPORT_BYTES];
} report_keyboard_t;

#define columnCount 10
#define rowCount 7
const int columnPins[columnCount] = { 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };
const int rowPins[rowCount] = { 12, 13, 14, 15, 16, 17, 18 };

report_keyboard_t report;

void setup() {
	for (int column = 0; column < columnCount; ++column) {
		pinMode(columnPins[column], INPUT);
		digitalWrite(columnPins[column], HIGH);
	}

	for (int row = 0; row < rowCount; ++row) {
		pinMode(rowPins[row], OUTPUT);
		digitalWrite(rowPins[row], HIGH);
	}

	Serial.begin(9600);

	delay(200);
}

void loop() {
	for (uint8_t code = 0x04 /* A */; code <= 0x27 /* 0 */; ++code) {
		toggleKeyBit(code);
	}
	report.modifiers ^= 0b01110111;
	Serial.write((const uint8_t*)&report, sizeof(report));
	delay(2000);
}

void setKeyBit(uint8_t code) {
	if ((code >> 3) < REPORT_BYTES) {
		report.keys[code >> 3] |= 1<<(code & 0b111);
	}
}

void unsetKeyBit(uint8_t code) {
	if ((code >> 3) < REPORT_BYTES) {
		report.keys[code >> 3] &= ~(1<<(code & 0b111));
	}
}

void toggleKeyBit(uint8_t code) {
	if ((code >> 3) < REPORT_BYTES) {
		report.keys[code >> 3] ^= 1<<(code & 0b111);
	}
}
