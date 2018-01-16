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
#define ALWAYS_INLINE __attribute__ ((always_inline))

// #define ATTR_ALIAS(func)           __attribute__ ((alias( #func )))
// #define ATTR_ALWAYS_INLINE         __attribute__ ((always_inline))
// #define ATTR_CONST                 __attribute__ ((const))
// #define ATTR_NON_NULL_PTR_ARG(...) __attribute__ ((nonnull (__VA_ARGS__)))
// #define ATTR_PACKED                __attribute__ ((packed))
// #define ATTR_UNUSED                __attribute__ ((unused))
// #define ATTR_WARN_UNUSED_RESULT    __attribute__ ((warn_unused_result))
// #define ATTR_WEAK                  __attribute__ ((weak))

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;

#define TWI_BASE_ADDRESS 0x58
#define TWI_ADDRESS_LEFT TWI_BASE_ADDRESS
#define TWI_ADDRESS_RIGHT (TWI_BASE_ADDRESS | 0x01)

#define TWI_CMD_SET_LEDS 0x10
#define TWI_CMD_GET_KEY_STATE 0x20

#define ROW_COUNT   5
#define COL_COUNT   7
#define LAYER_COUNT 2
#define LED_COUNT   10

#define GCC_MEMORY_BARRIER()       __asm__ __volatile__("" ::: "memory");
#define GCC_IS_COMPILE_CONST(x)    __builtin_constant_p(x)

#define enableGlobalInterrupts()  do { GCC_MEMORY_BARRIER(); sei(); GCC_MEMORY_BARRIER(); } while (0)
#define disableGlobalInterrupts() do { GCC_MEMORY_BARRIER(); cli(); GCC_MEMORY_BARRIER(); } while (0)

#define DISABLE_INTERRUPTS(code) do { \
	GCC_MEMORY_BARRIER(); \
	u8 __sreg = SREG; \
	cli(); \
	GCC_MEMORY_BARRIER(); \
	{ \
		code \
	} \
	GCC_MEMORY_BARRIER(); \
	SREG = __sreg; \
	GCC_MEMORY_BARRIER(); \
} while(0)
