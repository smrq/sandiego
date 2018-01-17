#pragma once

#include <avr/boot.h>
#include <avr/cpufunc.h>
#include <avr/interrupt.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/wdt.h>
#include <util/delay.h>
#include <util/twi.h>
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#define local static

#define PACKED __attribute__((__packed__))
#define UNUSED __attribute__ ((unused))

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;

#define TWI_BASE_ADDRESS  0x58
#define TWI_ADDRESS_LEFT  (TWI_BASE_ADDRESS | 0x00)
#define TWI_ADDRESS_RIGHT (TWI_BASE_ADDRESS | 0x01)

#define TWI_CMD_SET_LEDS      0x10
#define TWI_CMD_GET_KEY_STATE 0x20

#define ROW_COUNT   5
#define COL_COUNT   7
#define LAYER_COUNT 2
#define LED_COUNT   10

#define MEMORY_BARRIER __asm__ __volatile__("" ::: "memory");

#define enableGlobalInterrupts()  do { MEMORY_BARRIER; sei(); MEMORY_BARRIER; } while (0)
#define disableGlobalInterrupts() do { MEMORY_BARRIER; cli(); MEMORY_BARRIER; } while (0)
