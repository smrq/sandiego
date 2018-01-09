#pragma once

#include "defs.h"

typedef struct {
	u32 front[LED_COUNT];
	u32 back[LED_COUNT];
} led_buffer_t;

extern led_buffer_t leftLeds;
extern led_buffer_t rightLeds;

void transmitLeds(u8 address, led_buffer_t *buffer);
