#include "Arduino.h"
#include "Keys.h"

#define REPORT_SIZE 24
#define REPORT_BYTES (REPORT_SIZE-1)
#define SERIAL_RATE 9600

#define COLUMNS 10
#define ROWS 7

typedef struct {
	uint8_t modifiers;
	uint8_t keys[REPORT_BYTES];
} report_keyboard_t;

const int columnPins[COLUMNS] = { 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };
const int rowPins[ROWS] = { 12, 14, 15, 16, 17, 18, 19 };
const int ledPin = 13;

report_keyboard_t report;

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
	static uint8_t bitToFlip = K_A_a;
	static bool isLedLit = false;

	report.modifiers ^= K_MODIFIER_LSHIFT;
	toggleKeyBit(bitToFlip);

	if (++bitToFlip > K_0_RPAREN)
		bitToFlip = K_A_a;

	Serial.write((const uint8_t*)&report, sizeof(report));
	delay(500);
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
