#pragma once

#include <stdint.h>
#include <stdbool.h>

#define local static

#define u8 uint8_t
#define u16 uint16_t
#define u32 uint32_t

#define TWI_BASE_ADDRESS 0x58
#define TWI_CMD_SET_LED 0x10
#define TWI_CMD_SET_ALL_LEDS 0x11
#define TWI_CMD_SET_LED_PATTERN 0x12

#define LED_COUNT 10
