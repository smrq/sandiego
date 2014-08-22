#include "Arduino.h"

#include "Constants.h"
#include "Keys.h"
#include "Keymap.h"

typedef struct {
	uint8_t modifiers;
	uint8_t keys[REPORT_BYTES];
} report_keyboard_t;

const int rowPins[ROWS] = { 12, 14, 15, 16, 17, 18, 19 };
const int columnPins[COLUMNS] = { 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };
const int ledPin = 13;

const int keyMap[LAYERS][ROWS][COLUMNS] = {
	KEYMAP(
		CAPSLOCK, 1, 2, 3, 4, 5,                 6, 7, 8,     9,      0,         PAUSE_BREAK,
		GRAVE,    Q, W, E, R, T, INSERT, DELETE, Y, U, I,     O,      P,         EQUAL,
		PAGEUP,   A, S, D, F, G, TAB,    RETURN, H, J, K,     L,      SEMICOLON, QUOTE,
		PAGEDOWN, Z, X, C, V, B, HOME,   END,    N, M, COMMA, PERIOD, SLASH,     DASH,

		L1_SHIFT, LCTRL, LSHIFT, SPACE, BACKSPACE, LGUI,
		RALT, BACKSPACE, SPACE, RSHIFT, RCTRL, L1_SHIFT
	),
	KEYMAP(
		SCROLLLOCK, F1,   F2,   F3,   F4,   F5,               F6,     F7,     F8,       F9,       F10,       PRINTSCREEN_SYSRQ,
		NULL,       NULL, NULL, NULL, NULL, NULL, NULL, NULL, LBRACE, RBRACE, LBRACKET, RBRACKET, NULL,      F11,
		NULL,       NULL, NULL, NULL, NULL, NULL, NULL, NULL, LEFT,   DOWN,   UP,       RIGHT,    NULL,      F12,
		NULL,       NULL, NULL, NULL, NULL, NULL, NULL, NULL, PIPE,   NULL,   NULL,     NULL,     BACKSLASH, NULL,

		NULL, LCTRL, LSHIFT, SPACE, BACKSPACE, LGUI,
		RALT, BACKSPACE, SPACE, RSHIFT, RCTRL, NULL
	)
};

report_keyboard_t report;
report_keyboard_t lastReport;

bool ledPinLow = true;

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

int determineLayer(bool pressedKeys[][COLUMNS]) {
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

bool reportChanged() {
	if (report.modifiers != lastReport.modifiers)
		return true;
	for (int i = 0; i < REPORT_BYTES; ++i) {
		if (report.keys[i] != lastReport.keys[i])
			return true;
	}
	return false;
}

void loop() {
	bool pressedKeys[ROWS][COLUMNS];

	for (int r = 0; r < ROWS; ++r) {
		digitalWrite(rowPins[r], LOW);
		for (int c = 0; c < COLUMNS; ++c) {
			pressedKeys[r][c] = (digitalRead(columnPins[c]) == LOW);
		}
		digitalWrite(rowPins[r], HIGH);
	}

	int layer = determineLayer(pressedKeys);

	for (int r = 0; r < ROWS; ++r) {
		for (int c = 0; c < COLUMNS; ++c) {
			if (pressedKeys[r][c])
				setKeyBit(keyMap[layer][r][c]);
			else
				unsetKeyBit(keyMap[layer][r][c]);
		}
	}

	if (reportChanged()) {
		Serial.write((const uint8_t*)&report, sizeof(report));
		lastReport = report;
		digitalWrite(ledPin, (ledPinLow = !ledPinLow) ? LOW : HIGH);
	}
}

void setKeyBit(int code) {
	if (code >= K_LCTRL && code <= K_RGUI)
		report.modifiers |= 1<<(code - K_LCTRL);
	else if ((code >> 3) < REPORT_BYTES)
		report.keys[code >> 3] |= 1<<(code & 0x7);
}

void unsetKeyBit(int code) {
	if (code >= K_LCTRL && code <= K_RGUI)
		report.modifiers &= ~(1<<(code - K_LCTRL));
	else if ((code >> 3) < REPORT_BYTES)
		report.keys[code >> 3] &= ~(1<<(code & 0x7));
}