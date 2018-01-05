#include "defs.h"
#include "keys.h"

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
