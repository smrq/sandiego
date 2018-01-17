#pragma once

#include "defs.h"

typedef struct {
	u8 keys[32];
} PACKED USB_nkroKeyboardReport_t;

typedef struct {
	u8 modifiers;
	u8 reserved;
	u8 keyCodes[6];
} PACKED USB_bootKeyboardReport_t;

typedef u8 USB_ledReport_t;

bool populateBootKeyboardReport(USB_bootKeyboardReport_t *report);
bool populateNkroKeyboardReport(USB_nkroKeyboardReport_t *report);
void processLEDReport(USB_ledReport_t report);
