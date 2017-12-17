#pragma once

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>
#include <stdbool.h>

#define local static

#define PIN_SS 2
#define PIN_MOSI 3
#define PIN_SCK 5

#define TWI_BASE_ADDRESS 0x58

#define enableGlobalInterrupts sei
#define disableGlobalInterrupts cli

#define LED_COUNT 10

#define u8 uint8_t
#define u16 uint16_t
#define u32 uint32_t
