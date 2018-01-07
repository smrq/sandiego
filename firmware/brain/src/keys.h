#pragma once

#include "defs.h"

typedef struct {
	u8 front[ROW_COUNT];
	u8 back[ROW_COUNT];
} key_buffer_t;

extern key_buffer_t leftKeys;
extern key_buffer_t rightKeys;

void writeKeyRow(key_buffer_t *buffer, u8 row, u8 keys);
u8 readKeyRow(key_buffer_t *buffer, u8 row);
void flipKeyBuffer(key_buffer_t *buffer);
