#pragma once

#include "defs.h"
#include "usb-descriptors.h"

typedef struct {
	u8 keys[32];
} PACKED USB_nkroKeyboardReport_t;

typedef struct {
	u8 modifiers;
	u8 reserved;
	u8 keyCodes[6];
} PACKED USB_bootKeyboardReport_t;

typedef u8 USB_ledReport_t;

void populateBootKeyboardReport(USB_bootKeyboardReport_t *report);
void populateNkroKeyboardReport(USB_nkroKeyboardReport_t *report);
void processLEDReport(USB_ledReport_t report);
