#pragma once

#include "defs.h"

bool USB_getDescriptor(u8 type, u8 number, const void **address, u16 *size);
