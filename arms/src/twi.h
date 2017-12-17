#pragma once

#include <stdbool.h>
#include "defs.h"

#define TWI_BUFFER_SIZE 32

void TWI_init(u8 address, bool acceptGeneral);

extern void (*TWI_requestedData)(u8 *dest, u8 *length);
extern void (*TWI_receivedData)(u8 *src, u8 length);
