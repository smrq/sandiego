#include "defs.h"
#include "./leds.h"
#include "twi/twi.h"

led_buffer_t leftLeds = { 0 };
led_buffer_t rightLeds = { 0 };

local void flipLedBuffer(led_buffer_t *buffer) {
	memcpy(&buffer->front, &buffer->back, sizeof(buffer->front));
}

void transmitLeds(u8 address, led_buffer_t *buffer) {
	flipLedBuffer(buffer);
	while (!TWI_setLeds(address, buffer));
}
