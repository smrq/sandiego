#include "defs.h"
#include "leds.h"

led_buffer_t leftLeds = { 0 };
led_buffer_t rightLeds = { 0 };

void writeLed(led_buffer_t *buffer, u8 index, u32 color) {
	buffer->back[index] = color;
}

void writeAllLeds(led_buffer_t *buffer, u32 color) {
	for (u8 index = 0; index < LED_COUNT; ++index) {
		buffer->back[index] = color;
	}
}

void writeLedPattern(led_buffer_t *buffer, u32 *colors) {
	for (u8 index = 0; index < LED_COUNT; ++index) {
		buffer->back[index] = colors[index];
	}
}

u32 readLed(led_buffer_t *buffer, u8 index) {
	return buffer->front[index];
}

void flipLedBuffer(led_buffer_t *buffer) {
	for (u8 index = 0; index < LED_COUNT; ++index) {
		buffer->front[index] = buffer->back[index];
	}
}
