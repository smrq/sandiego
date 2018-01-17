#include "defs.h"
#include "./usb.h"
#include "keymap.h"
#include "buffers/keys.h"

local USB_nkroKeyboardReport_t previousReport;

bool populateBootKeyboardReport(USB_bootKeyboardReport_t *report) {
	USB_nkroKeyboardReport_t nkroReport = { 0 };
	bool hasUpdated = populateNkroKeyboardReport(&nkroReport);

	// Modifiers are 0xE0-0xE7, so they take up exactly one page of the NKRO report. Thanks, HID spec!
	report->modifiers = nkroReport.keys[0xE0 / 8];

	for (u8 keyCodeIndex = 0, page = 0; page < 32 && keyCodeIndex < 6; ++page) {
		// Skip modifier key page
		if (page == 0xE0 / 8) {
			continue;
		}

		if (nkroReport.keys[page]) {
			for (u8 pageIndex = 0; pageIndex < 8 && keyCodeIndex < 6; ++pageIndex) {
				if (nkroReport.keys[page] & _BV(pageIndex)) {
					report->keyCodes[keyCodeIndex++] = (page * 8) + pageIndex;
				}
			}
		}
	}

	return hasUpdated;
}

local void setNkroKey(USB_nkroKeyboardReport_t *report, u8 key) {
	report->keys[key / 8] |= _BV(key % 8);
}

local u8 determineKeymapLayer() {
	u8 layer = 0;
	for (u8 row = 0; row < ROW_COUNT; ++row) {
		if (leftKeys.front[row] || rightKeys.front[row]) {
			for (u8 col = 0; col < COL_COUNT; ++col) {
				if (leftKeys.front[row] & _BV(col)) {
					u16 mapping = pgm_read_word(&keymap.layers[layer].left[row][col]);
					if (mapping & KEYMAP_LAYER_SHIFT) {
						layer = mapping & KEYMAP_CODE_MASK;
					}
				}
				if (rightKeys.front[row] & _BV(col)) {
					u16 mapping = pgm_read_word(&keymap.layers[layer].right[row][col]);
					if (mapping & KEYMAP_LAYER_SHIFT) {
						layer = mapping & KEYMAP_CODE_MASK;
					}
				}
			}
		}
	}
	return layer;
}

void setNkroKeyForHexValue(USB_nkroKeyboardReport_t *report, u8 value) {
	if (value == 0x0) {
		setNkroKey(report, HID_KEYPAD_0_INSERT);
	} else if (value < 0xA) {
		setNkroKey(report, HID_KEYPAD_1_END + (value - 0x1));
	} else {
		setNkroKey(report, HID_KEYBOARD_A + (value - 0xA));
	}
}

bool populateNkroKeyboardReport(USB_nkroKeyboardReport_t *report) {
	u8 layer = determineKeymapLayer();

	if (leftKeys.front[0]) setNkroKey(report, HID_KEYBOARD_A);
	if (leftKeys.front[1]) setNkroKey(report, HID_KEYBOARD_B);
	if (leftKeys.front[2]) setNkroKey(report, HID_KEYBOARD_C);
	if (leftKeys.front[3]) setNkroKey(report, HID_KEYBOARD_D);
	if (leftKeys.front[4]) setNkroKey(report, HID_KEYBOARD_E);

	bool hasUpdated = memcmp(&previousReport, report, sizeof(previousReport)) != 0;
	memcpy(&previousReport, report, sizeof(previousReport));
	return hasUpdated;
}

void processLEDReport(USB_ledReport_t LEDReport) {
	u8 value = _BV(1);

	if (LEDReport & HID_LED_NUM_LOCK) {
		value |= _BV(0);
	}

	if (LEDReport & HID_LED_CAPS_LOCK) {
		value |= _BV(2);
	}

	if (LEDReport & HID_LED_SCROLL_LOCK) {
		value |= _BV(3);
	}

	// debug_output(value);
}
