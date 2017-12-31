#pragma once

#include "defs.h"

#define TWI_BUFFER_SIZE 32

void TWI_init();
void TWI_transmit(u8 address, u8 *buffer, u8 length);
bool TWI_busy();
