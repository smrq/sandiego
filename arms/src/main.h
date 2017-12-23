#pragma once

#include "defs.h"

typedef struct {
	u8 type;
	union {
		struct {
			u8 index;
			u32 color;
		} setLed;

		struct {
			u32 color;
		} setAllLeds;

		struct {
			u32 colors[LED_COUNT];
		} setLedPattern;
	} data;
} twiCommand_t;
