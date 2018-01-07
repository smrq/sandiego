#pragma once

#include "defs.h"

void led_setColor(u8 index, u32 color);
u8 led_getRedValue(u8 index);
u8 led_getGreenValue(u8 index);
u8 led_getBlueValue(u8 index);
u8 led_getBrightnessValue(u8 index);
