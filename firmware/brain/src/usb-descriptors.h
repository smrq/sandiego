#pragma once

#include <LUFA/Drivers/USB/USB.h>
#include "defs.h"

#define KEYBOARD_IN_EPADDR (ENDPOINT_DIR_IN  | 1)
#define KEYBOARD_OUT_EPADDR (ENDPOINT_DIR_OUT | 2)
#define KEYBOARD_EPSIZE 32

uint16_t CALLBACK_USB_GetDescriptor(
	const u16 wValue,
    const u16 wIndex,
    const void** const DescriptorAddress);
