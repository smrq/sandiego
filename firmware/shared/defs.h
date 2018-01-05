#pragma once

#include <avr/interrupt.h>
#include <stdint.h>
#include <stdbool.h>

#define local static
#define enableGlobalInterrupts sei
#define disableGlobalInterrupts cli

#define PACKED __attribute__((__packed__))
#define UNUSED __attribute__ ((unused))

#define u8 uint8_t
#define u16 uint16_t
#define u32 uint32_t

#define TWI_BASE_ADDRESS 0x58
#define TWI_CMD_SET_LED 0x10
#define TWI_CMD_SET_ALL_LEDS 0x20
#define TWI_CMD_SET_LEDS 0x30
#define TWI_CMD_GET_KEY_STATE 0x40

#define LED_COUNT 10

#define ROW_COUNT 5
#define COL_COUNT 7
