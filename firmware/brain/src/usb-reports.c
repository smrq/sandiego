#include <LUFA/Drivers/USB/USB.h>
#include "defs.h"
#include "debug.h"
#include "keys.h"
#include "usb-reports.h"

void populateBootKeyboardReport(USB_bootKeyboardReport_t *report) {
	// TODO: set up real keyboard matrix
	if (leftKeys.front[0]) {
		report->modifiers = HID_KEYBOARD_MODIFIER_LEFTSHIFT;
	}

	u8 keyCodeIndex = 0;
	if (leftKeys.front[1]) {
		report->keyCodes[keyCodeIndex++] = HID_KEYBOARD_SC_LEFT_ARROW;
	}
	if (leftKeys.front[2]) {
		report->keyCodes[keyCodeIndex++] = HID_KEYBOARD_SC_RIGHT_ARROW;
	}
	if (leftKeys.front[3]) {
		report->keyCodes[keyCodeIndex++] = HID_KEYBOARD_SC_A;
	}
	if (leftKeys.front[4]) {
		report->keyCodes[keyCodeIndex++] = HID_KEYBOARD_SC_B;
	}
}

local void setNkroKey(USB_nkroKeyboardReport_t *report, u8 key) {
	report->keys[key / 8] |= _BV(key % 8);
}

void populateNkroKeyboardReport(USB_nkroKeyboardReport_t *report) {
	// TODO: set up real keyboard matrix
	if (leftKeys.front[0]) {
		setNkroKey(report, HID_KEYBOARD_MODIFIER_LEFTSHIFT);
	}
	if (leftKeys.front[1]) {
		setNkroKey(report, HID_KEYBOARD_SC_A);
		setNkroKey(report, HID_KEYBOARD_SC_B);
		setNkroKey(report, HID_KEYBOARD_SC_C);
		setNkroKey(report, HID_KEYBOARD_SC_D);
		setNkroKey(report, HID_KEYBOARD_SC_E);
		setNkroKey(report, HID_KEYBOARD_SC_F);
		setNkroKey(report, HID_KEYBOARD_SC_G);
		setNkroKey(report, HID_KEYBOARD_SC_H);
		setNkroKey(report, HID_KEYBOARD_SC_I);
		setNkroKey(report, HID_KEYBOARD_SC_J);
		setNkroKey(report, HID_KEYBOARD_SC_K);
		setNkroKey(report, HID_KEYBOARD_SC_L);
		setNkroKey(report, HID_KEYBOARD_SC_M);
		setNkroKey(report, HID_KEYBOARD_SC_N);
		setNkroKey(report, HID_KEYBOARD_SC_O);
		setNkroKey(report, HID_KEYBOARD_SC_P);
		setNkroKey(report, HID_KEYBOARD_SC_Q);
		setNkroKey(report, HID_KEYBOARD_SC_R);
		setNkroKey(report, HID_KEYBOARD_SC_S);
		setNkroKey(report, HID_KEYBOARD_SC_T);
		setNkroKey(report, HID_KEYBOARD_SC_U);
		setNkroKey(report, HID_KEYBOARD_SC_V);
		setNkroKey(report, HID_KEYBOARD_SC_W);
		setNkroKey(report, HID_KEYBOARD_SC_X);
		setNkroKey(report, HID_KEYBOARD_SC_Y);
		setNkroKey(report, HID_KEYBOARD_SC_Z);
	}
	if (leftKeys.front[2]) {
		setNkroKey(report, HID_KEYBOARD_SC_B);
	}
	if (leftKeys.front[3]) {
		setNkroKey(report, HID_KEYBOARD_SC_C);
	}
	if (leftKeys.front[4]) {
		setNkroKey(report, HID_KEYBOARD_SC_D);
	}
}

void processLEDReport(USB_ledReport_t LEDReport)
{
	u8 value = _BV(1);

	if (LEDReport & HID_KEYBOARD_LED_NUMLOCK) {
		value |= _BV(0);
	}

	if (LEDReport & HID_KEYBOARD_LED_CAPSLOCK) {
		value |= _BV(2);
	}

	if (LEDReport & HID_KEYBOARD_LED_SCROLLLOCK) {
		value |= _BV(3);
	}

	debug_output(value);
}
