#include "Arduino.h"

#include "Constants.h"
#include "Keys.h"
#include "Keymap.h"

const int rowPins[ROWS] = { 12, 14, 15, 16, 17, 18, 19 };
const int columnPins[COLUMNS] = { 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };
const int ledPin = 13;

const int keyMap[LAYERS][ROWS][COLUMNS] = {
	KEYMAP(
		F1,          F2, F3, F4, F5, F6,                 F7, F8, F9,           F10,           F11,             F12,
		ESC,         Q,  W,  E,  R,  T,  NULL,   NULL,   Y,  U,  I,            O,             P,               BACKSPACE,
		GRAVE_TILDE, A,  S,  D,  F,  G,  TAB,    RETURN, H,  J,  K,            L,             SEMICOLON_COLON, QUOTE_DOUBLE,
		PRINTSCREEN, Z,  X,  C,  V,  B,  INSERT, DELETE, N,  M,  COMMA_LANGLE, PERIOD_RANGLE, SLASH_QUESTION,  DASH_UNDERSCORE,

		LAYER_SHIFT(1), LCTRL, LSHIFT, SPACE, LGUI, LALT,
		RALT, RGUI, SPACE, RSHIFT, RCTRL, LAYER_SHIFT(1)
	),
	KEYMAP(
		F13,    F14,     F15,  F16,  F17,  F18,                            F19,    F20,      F21,    F22,    F23,       F24,
		DOLLAR, CARET,   7,    8,    9,    ASTERISK, NULL,       NULL,     LPAREN, RPAREN,   LBRACE, RBRACE, LBRACKET,  RBRACKET,
		HASH,   PERCENT, 4,    5,    6,    PLUS,     NUMLOCK,    CAPSLOCK, LEFT,   DOWN,     UP,     RIGHT,  BANG,      AMPERSAND,
		AT,     0,       1,    2,    3,    EQUAL,    SCROLLLOCK, PAUSE,    HOME,   PAGEDOWN, PAGEUP, END,    BACKSLASH, PIPE,

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

void scanKeys(bool pressedKeys[][COLUMNS], int *layer) {
	for (int r = 0; r < ROWS; ++r) {
		digitalWrite(rowPins[r], LOW);
		for (int c = 0; c < COLUMNS; ++c) {
			bool keyDown = digitalRead(columnPins[c]) == LOW;
			pressedKeys[r][c] = keyDown;

			// we can check for layer shifts while scanning physical keys because
			// the intermediate result doesn't rely on any keys not yet scanned
			if (keyDown && keyMap[*layer][r][c] & KB_LAYER_SHIFT) {
				*layer = keyMap[*layer][r][c] & KB_CODE_MASK;
			}
		}
		digitalWrite(rowPins[r], HIGH);
	}
}

void convertToVirtualKeys(
	int layer,
	int *baseLayer,
	const bool pressedKeys[][COLUMNS],
	const bool lastPressedKeys[ROWS][COLUMNS],
	bool pressedVirtualKeys[][ROWS][COLUMNS])
{
	for (int r = 0; r < ROWS; ++r) {
	for (int c = 0; c < COLUMNS; ++c) {
		if (pressedKeys[r][c] && !lastPressedKeys[r][c]) {
			pressedVirtualKeys[layer][r][c] = true;

			// we can check for layer switches now because the layer for
			// this frame has already been determined during the physical
			// key scan.  if the base layer changes then it will affect
			// keypresses starting with the next frame.
			if (keyMap[layer][r][c] & KB_LAYER_SWITCH) {
				*baseLayer = keyMap[layer][r][c] & KB_CODE_MASK;
			}
		}
		else if (!pressedKeys[r][c] && lastPressedKeys[r][c]) {
			// when a physical key is released, all virtual keys at that
			// row,col coordinate should be released.  if the physical key
			// was pressed andthen a layer change happened, then a virtual
			// key will still be down on the old layer, so we need to modify
			// more than just the current layer.
			for (int l = 0; l < LAYERS; l++) {
				pressedVirtualKeys[l][r][c] = false;
			}
		}
	}}
}

bool isShiftDown(const bool pressedVirtualKeys[][ROWS][COLUMNS]) {
	for (int l = 0; l < LAYERS; ++l) {
	for (int r = 0; r < ROWS; ++r) {
	for (int c = 0; c < COLUMNS; ++c) {
		if (pressedVirtualKeys[l][r][c] &&
			((keyMap[l][r][c] == K_LSHIFT) ||
			(keyMap[l][r][c] == K_RSHIFT)))
		{
			return true;
		}
	}}}
	return false;
}

void setKeyBit(uint8_t report[], int code) {
	if ((code >> 3) < REPORT_SIZE)
		report[code >> 3] |= 1<<(code & 0x7);
}

void unsetKeyBit(uint8_t report[], int code) {
	if ((code >> 3) < REPORT_SIZE)
		report[code >> 3] &= ~(1<<(code & 0x7));
}

void populateReport(
	const bool pressedKeys[][COLUMNS],
	const bool lastPressedKeys[ROWS][COLUMNS],
	const bool pressedVirtualKeys[][ROWS][COLUMNS],
	bool shifted,
	uint8_t report[],
	uint8_t alternateShiftReport[],
	bool *isAlternateShiftReportUsed)
{
	*isAlternateShiftReportUsed = false;

	for (int l = 0; l < LAYERS; ++l) {
	for (int r = 0; r < ROWS; ++r) {
	for (int c = 0; c < COLUMNS; ++c) {
		if (pressedVirtualKeys[l][r][c] &&
			keyMap[l][r][c] & (KB_HARDWARE_KEY | KB_UNSHIFTED_KEY | KB_SHIFTED_KEY))
		{
			if (keyMap[l][r][c] & KB_HARDWARE_KEY) {
				setKeyBit(report, keyMap[l][r][c] & KB_CODE_MASK);
				setKeyBit(alternateShiftReport, keyMap[l][r][c] & KB_CODE_MASK);
			}
			else if ((!shifted && keyMap[l][r][c] & KB_UNSHIFTED_KEY) ||
				(shifted && keyMap[l][r][c] & KB_SHIFTED_KEY))
			{
				setKeyBit(report, keyMap[l][r][c] & KB_CODE_MASK);
			}
			else if ((shifted && keyMap[l][r][c] & KB_UNSHIFTED_KEY) ||
				(!shifted && keyMap[l][r][c] & KB_SHIFTED_KEY))
			{
				setKeyBit(report, keyMap[l][r][c] & KB_CODE_MASK);
				setKeyBit(alternateShiftReport, keyMap[l][r][c] & KB_CODE_MASK);

				// we only need the alternate shift report if there is a key with
				// the opposite shift setting that has just been pressed this frame
				if (pressedKeys[r][c] && !lastPressedKeys[r][c]) {
					*isAlternateShiftReportUsed = true;
				}
			}
		}
	}}}
/*
	if (*isAlternateShiftReportUsed) {
		if (shifted) {
			unsetKeyBit(alternateShiftReport, KEY_USB_LSHIFT);
			unsetKeyBit(alternateShiftReport, KEY_USB_RSHIFT);
		}
		else {
			setKeyBit(alternateShiftReport, KEY_USB_LSHIFT);
		}
	}
*/
}

void toggleLED() {
	static bool ledPinLow = true;
	digitalWrite(ledPin, (ledPinLow = !ledPinLow) ? LOW : HIGH);
}

void toggleShiftStatus(uint8_t report[]) {
	bool lShiftDown = !!(report[KEY_USB_LSHIFT >> 3] & (1 << (KEY_USB_LSHIFT & 0x7)));
	bool rShiftDown = !!(report[KEY_USB_RSHIFT >> 3] & (1 << (KEY_USB_RSHIFT & 0x7)));

	if (lShiftDown || rShiftDown) {
		unsetKeyBit(report, KEY_USB_LSHIFT);
		unsetKeyBit(report, KEY_USB_RSHIFT);
	} else {
		setKeyBit(report, KEY_USB_LSHIFT);
	}
}

void sendReport(const uint8_t report[], const uint8_t alternateShiftReport[]) {
	static uint8_t lastReport[REPORT_SIZE] = { 0 };

	if (alternateShiftReport != NULL) {
		uint8_t toggledLastReport[REPORT_SIZE];
		memcpy(toggledLastReport, lastReport, REPORT_SIZE);
		toggleShiftStatus(toggledLastReport);
		Serial.write(toggledLastReport, REPORT_SIZE);

		uint8_t toggledAlternateShiftReport[REPORT_SIZE];
		memcpy(toggledAlternateShiftReport, alternateShiftReport, REPORT_SIZE);
		toggleShiftStatus(toggledAlternateShiftReport);
		Serial.write(toggledAlternateShiftReport, REPORT_SIZE);

		Serial.write(alternateShiftReport, REPORT_SIZE);
	}
	Serial.write(report, REPORT_SIZE);
	toggleLED();
	memcpy(lastReport, report, REPORT_SIZE);
}

void loop() {
	static int baseLayer = 0;
	static bool pressedVirtualKeys[LAYERS][ROWS][COLUMNS] = { 0 };
	static bool lastPressedKeys[ROWS][COLUMNS] = { 0 };
	bool pressedKeys[ROWS][COLUMNS] = { 0 };
	uint8_t report[REPORT_SIZE] = { 0 };
	uint8_t alternateShiftReport[REPORT_SIZE] = { 0 };

	int layer = baseLayer;
	scanKeys(pressedKeys, &layer);

	if (memcmp(lastPressedKeys, pressedKeys, sizeof(pressedKeys)) == 0)
		return;

	convertToVirtualKeys(layer, &baseLayer, pressedKeys, lastPressedKeys, pressedVirtualKeys);
	bool shifted = isShiftDown(pressedVirtualKeys);
	bool isAlternateShiftReportUsed;
	populateReport(pressedKeys, lastPressedKeys, pressedVirtualKeys, shifted, report, alternateShiftReport, &isAlternateShiftReportUsed);
	sendReport(report, isAlternateShiftReportUsed ? alternateShiftReport : NULL);

	memcpy(lastPressedKeys, pressedKeys, sizeof(pressedKeys));
}
