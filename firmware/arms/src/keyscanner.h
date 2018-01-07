#pragma once

#include "defs.h"

typedef struct {
	u8 current;
	u8 counterHigh;
	u8 counterLow;
} keyscanner_rowstate_t;

void keyscanner_scan();
u8 keyscanner_getRowState(u8 row);
