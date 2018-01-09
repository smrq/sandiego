#pragma once

#include <LUFA/Drivers/USB/USB.h>
#include "defs.h"

#define KEYBOARD_IN_EPADDR (ENDPOINT_DIR_IN | 1)
#define KEYBOARD_OUT_EPADDR (ENDPOINT_DIR_OUT | 2)
#define KEYBOARD_EPSIZE 32

#define VENDOR_ID      0xF055
#define PRODUCT_ID     0x2042
#define RELEASE_NUMBER VERSION_BCD(2,0,0)
