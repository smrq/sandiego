#pragma once

#include "defs.h"
#include "leds.h"
#include "keys.h"

void TWI_init();
bool TWI_busy();
bool TWI_setLeds(u8 address, led_buffer_t *buffer);
bool TWI_getKeyState(u8 address, key_buffer_t *buffer);
