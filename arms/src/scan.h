#pragma once

#include "defs.h"

typedef struct {
	u8 current;
	u8 counterHigh;
	u8 counterLow;
} rowstate_t;

void scan();
u8 getRowState(u8 row);
