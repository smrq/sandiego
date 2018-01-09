#include "defs.h"
#include "debug.h"
#include "keys.h"
#include "twi.h"

key_buffer_t leftKeys = { 0 };
key_buffer_t rightKeys = { 0 };

local void flipKeyBuffer(key_buffer_t *buffer) {
	memcpy(&buffer->front, &buffer->back, sizeof(buffer->front));
}

void requestKeys(u8 address, key_buffer_t *buffer) {
	while (!TWI_getKeyState(address, buffer));
	while (TWI_busy()); // wait for transmission end synchronously
	flipKeyBuffer(buffer);
}
