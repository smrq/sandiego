#pragma once

#include "defs.h"

#define TWI_BUFFER_SIZE 32

void TWI_init(u8 address, bool acceptGeneral);

void TWI_requestedData(u8 *dest, u8 *length);
