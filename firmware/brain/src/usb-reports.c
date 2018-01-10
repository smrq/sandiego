#include "defs.h"
#include "debug.h"
#include "keys.h"
#include "usb.h"

void populateBootKeyboardReport(USB_bootKeyboardReport_t *report) {
	// TODO: set up real keyboard matrix
	if (leftKeys.front[0]) {
		report->modifiers = HID_KEYBOARD_LEFT_SHIFT;
	}

	u8 keyCodeIndex = 0;
	if (leftKeys.front[1]) {
		report->keyCodes[keyCodeIndex++] = HID_KEYBOARD_LEFT_ARROW;
	}
	if (leftKeys.front[2]) {
		report->keyCodes[keyCodeIndex++] = HID_KEYBOARD_RIGHT_ARROW;
	}
	if (leftKeys.front[3]) {
		report->keyCodes[keyCodeIndex++] = HID_KEYBOARD_A;
	}
	if (leftKeys.front[4]) {
		report->keyCodes[keyCodeIndex++] = HID_KEYBOARD_B;
	}
}

local void setNkroKey(USB_nkroKeyboardReport_t *report, u8 key) {
	report->keys[key / 8] |= _BV(key % 8);
}

void populateNkroKeyboardReport(USB_nkroKeyboardReport_t *report) {
	// TODO: set up real keyboard matrix
	if (leftKeys.front[0]) {
		setNkroKey(report, HID_KEYBOARD_LEFT_SHIFT);
	}
	if (leftKeys.front[1]) {
		setNkroKey(report, HID_KEYBOARD_A);
		setNkroKey(report, HID_KEYBOARD_B);
		setNkroKey(report, HID_KEYBOARD_C);
		setNkroKey(report, HID_KEYBOARD_D);
		setNkroKey(report, HID_KEYBOARD_E);
		setNkroKey(report, HID_KEYBOARD_F);
		setNkroKey(report, HID_KEYBOARD_G);
		setNkroKey(report, HID_KEYBOARD_H);
		setNkroKey(report, HID_KEYBOARD_I);
		setNkroKey(report, HID_KEYBOARD_J);
		setNkroKey(report, HID_KEYBOARD_K);
		setNkroKey(report, HID_KEYBOARD_L);
		setNkroKey(report, HID_KEYBOARD_M);
		setNkroKey(report, HID_KEYBOARD_N);
		setNkroKey(report, HID_KEYBOARD_O);
		setNkroKey(report, HID_KEYBOARD_P);
		setNkroKey(report, HID_KEYBOARD_Q);
		setNkroKey(report, HID_KEYBOARD_R);
		setNkroKey(report, HID_KEYBOARD_S);
		setNkroKey(report, HID_KEYBOARD_T);
		setNkroKey(report, HID_KEYBOARD_U);
		setNkroKey(report, HID_KEYBOARD_V);
		setNkroKey(report, HID_KEYBOARD_W);
		setNkroKey(report, HID_KEYBOARD_X);
		setNkroKey(report, HID_KEYBOARD_Y);
		setNkroKey(report, HID_KEYBOARD_Z);
	}
	if (leftKeys.front[2]) {
		setNkroKey(report, HID_KEYBOARD_B);
	}
	if (leftKeys.front[3]) {
		setNkroKey(report, HID_KEYBOARD_C);
	}
	if (leftKeys.front[4]) {
		setNkroKey(report, HID_KEYBOARD_D);
	}
}

void processLEDReport(USB_ledReport_t LEDReport)
{
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

	debug_output(value);
}
