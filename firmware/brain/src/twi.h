#pragma once

#include "defs.h"

#define TWI_BUFFER_SIZE 256

void TWI_init();
bool TWI_transmit(u8 address, u8 *buffer, u8 length);
bool TWI_busy();
