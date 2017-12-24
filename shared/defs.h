#pragma once

#include <avr/interrupt.h>
#include <stdint.h>
#include <stdbool.h>

#define local static
#define enableGlobalInterrupts sei
#define disableGlobalInterrupts cli

#define u8 uint8_t
#define u16 uint16_t
#define u32 uint32_t

#define PIN_SS 2
#define PIN_MOSI 3
#define PIN_SCK 5

#define TWI_BASE_ADDRESS 0x58
#define TWI_CMD_SET_LED 0x10
#define TWI_CMD_SET_ALL_LEDS 0x11
#define TWI_CMD_SET_LED_BANK 0x12

#define LED_COUNT 10
#define LED_BANK_SIZE 4
#define LED_BANKS 3
