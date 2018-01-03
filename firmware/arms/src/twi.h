#pragma once

#include "defs.h"

#define TWI_BUFFER_SIZE 8

void TWI_init(u8 address, bool acceptGeneral);
