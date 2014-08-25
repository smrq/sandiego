#include "Arduino.h"

#include "Constants.h"
#include "Keys.h"
#include "Keymap.h"

const int rowPins[ROWS] = { 12, 14, 15, 16, 17, 18, 19 };
const int columnPins[COLUMNS] = { 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };
const int ledPin = 13;

const int keyMap[LAYERS][ROWS][COLUMNS] = {
	KEYMAP(
		GRAVE,    1, 2, 3, 4, 5,                 6, 7, 8,     9,      0,         BACKSPACE,
		ESC,      Q, W, E, R, T, INSERT, DELETE, Y, U, I,     O,      P,         EQUAL,
		PAGEUP,   A, S, D, F, G, TAB,    RETURN, H, J, K,     L,      SEMICOLON, QUOTE,
		PAGEDOWN, Z, X, C, V, B, HOME,   END,    N, M, COMMA, PERIOD, SLASH,     DASH,

		L1_SHIFT, LCTRL, LSHIFT, SPACE, LGUI, LALT,
		RALT, RGUI, SPACE, RSHIFT, RCTRL, L1_SHIFT
	),
	KEYMAP(
		NULL,       F1,   F2,   F3,   F4,   F5,               F6,       F7,       F8,   F9,    F10,       PAUSE_BREAK,
		NUMLOCK,    NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,     NULL,     NULL, NULL,  NULL,      F11,
		CAPSLOCK,   NULL, NULL, NULL, NULL, NULL, NULL, NULL, LEFT,     DOWN,     UP,   RIGHT, NULL,      F12,
		SCROLLLOCK, NULL, NULL, NULL, NULL, NULL, NULL, NULL, LBRACKET, RBRACKET, NULL, NULL,  BACKSLASH, PRINTSCREEN_SYSRQ,

		NULL, LCTRL, LSHIFT, SPACE, LGUI, LALT,
		RALT, RGUI, SPACE, RSHIFT, RCTRL, NULL
	)
};

void setup() {
	initializePins();
	Serial.begin(SERIAL_RATE);
	delay(200);
}

void initializePins() {
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
}

void scanKeys(bool pressedKeys[][COLUMNS]) {
	for (int r = 0; r < ROWS; ++r) {
		digitalWrite(rowPins[r], LOW);
		for (int c = 0; c < COLUMNS; ++c) {
			pressedKeys[r][c] = (digitalRead(columnPins[c]) == LOW);
		}
		digitalWrite(rowPins[r], HIGH);
	}
}

int determineLayer(const bool pressedKeys[][COLUMNS]) {
	int layer = 0;
	for (int r = 0; r < ROWS; ++r) {
		for (int c = 0; c < COLUMNS; ++c) {
			if (pressedKeys[r][c]) {
				switch (keyMap[layer][r][c]) {
					case K_L0_SHIFT: layer = 0; break;
					case K_L1_SHIFT: layer = 1; break;
					case K_L2_SHIFT: layer = 2; break;
					case K_L3_SHIFT: layer = 3; break;
				}
			}
		}
	}
	return layer;
}

void setKeyBit(uint8_t report[], int code) {
	if ((code >> 3) < REPORT_SIZE)
		report[code >> 3] |= 1<<(code & 0x7);
}

void populateReport(const bool pressedKeys[][COLUMNS], int layer, uint8_t report[]) {
	for (int r = 0; r < ROWS; ++r) {
		for (int c = 0; c < COLUMNS; ++c) {
			if (pressedKeys[r][c])
				setKeyBit(report, keyMap[layer][r][c]);
		}
	}
}

void toggleLED() {
	static bool ledPinLow = true;
	digitalWrite(ledPin, (ledPinLow = !ledPinLow) ? LOW : HIGH);
}

void loop() {
	static uint8_t lastReport[REPORT_SIZE] = { 0 };
	bool pressedKeys[ROWS][COLUMNS] = { 0 };
	uint8_t report[REPORT_SIZE] = { 0 };

	scanKeys(pressedKeys);
	int layer = determineLayer(pressedKeys);
	populateReport(pressedKeys, layer, report);
	
	if (memcmp(lastReport, report, REPORT_SIZE) != 0) {
		Serial.write(report, REPORT_SIZE);
		memcpy(lastReport, report, REPORT_SIZE);
		toggleLED();
	}
}
