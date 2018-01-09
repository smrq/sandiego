#pragma once

#include "defs.h"

typedef struct {
	u8 front[ROW_COUNT];
	u8 back[ROW_COUNT];
} key_buffer_t;

extern key_buffer_t leftKeys;
extern key_buffer_t rightKeys;

void requestKeys(u8 address, key_buffer_t *buffer);
