#include "defs.h"
#include "debug.h"
#include "keys.h"
#include "twi.h"

key_buffer_t leftKeys = { 0 };
key_buffer_t rightKeys = { 0 };

void writeKeyRow(key_buffer_t *buffer, u8 row, u8 keys) {
	buffer->back[row] = keys;
}

u8 readKeyRow(key_buffer_t *buffer, u8 row) {
	return buffer->front[row];
}

void flipKeyBuffer(key_buffer_t *buffer) {
	for (u8 index = 0; index < ROW_COUNT; ++index) {
		buffer->front[index] = buffer->back[index];
	}
}

void requestKeys(u8 address, key_buffer_t *buffer) {
	while (!TWI_getKeyState(address, buffer));
	while (TWI_busy()); // wait for transmission end synchronously
	flipKeyBuffer(buffer);
}
