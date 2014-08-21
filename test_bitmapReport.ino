#include "Arduino.h"

#define REPORT_SIZE 8
#define REPORT_BYTES (REPORT_SIZE-1)
#define SERIAL_RATE 9600

typedef struct {
	uint8_t modifiers;
	uint8_t keys[REPORT_BYTES];
} report_keyboard_t;

#define COLUMNS 10
#define ROWS 7
const int columnPins[COLUMNS] = { 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };
const int rowPins[ROWS] = { 12, 14, 15, 16, 17, 18, 19 };

const int ledPin = 13;

report_keyboard_t report;
bool isLedLit = false;

void setup() {
	for (int column = 0; column < COLUMNS; ++column) {
		pinMode(columnPins[column], INPUT);
		digitalWrite(columnPins[column], HIGH);
	}

	for (int row = 0; row < ROWS; ++row) {
		pinMode(rowPins[row], OUTPUT);
		digitalWrite(rowPins[row], HIGH);
	}

	pinMode(ledPin, OUTPUT);
	digitalWrite(ledPin, LOW);

	Serial.begin(SERIAL_RATE);

	delay(200);
}

void loop() {
	toggleKeyBit(0x48);
	Serial.write((const uint8_t*)&report, sizeof(report));

	isLedLit = !isLedLit;
	digitalWrite(ledPin, isLedLit ? HIGH : LOW);

	delay(750);
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
