#pragma once

#include "defs.h"

typedef struct {
	u32 front[LED_COUNT];
	u32 back[LED_COUNT];
} led_buffer_t;

extern led_buffer_t leftLeds;
extern led_buffer_t rightLeds;

void writeLed(led_buffer_t *buffer, u8 index, u32 color);
void writeAllLeds(led_buffer_t *buffer, u32 color);
void writeLedPattern(led_buffer_t *buffer, u32 *colors);
u32 readLed(led_buffer_t *buffer, u8 index);
void flipLedBuffer(led_buffer_t *buffer);
void transmitLeds(u8 address, led_buffer_t *buffer);
