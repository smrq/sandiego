#include <avr/pgmspace.h>
#include "defs.h"
#include "led.h"

local bool transmitAgain = false;

local volatile enum TransmitState {
	IDLE,
	START_FRAME,
	LED_FRAME,
	END_FRAME
} transmitState = IDLE;
local volatile u16 transmitIndex = 0;

local volatile u32 ledColors[LED_COUNT] = {
	0x04FF0000,
	0x04FFFF00,
	0x0400FF00,
	0x0400FFFF,
	0x040000FF,
	0x04FF00FF,
	0x04FF4444,
	0x0444FF44,
	0x044444FF,
	0x04FFFFFF
};

local const u8 PROGMEM gamma8[] = {
	  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  1,  1,  1,
	  1,  1,  1,  1,  1,  1,  1,  1,  1,  2,  2,  2,  2,  2,  2,  2,
	  2,  3,  3,  3,  3,  3,  3,  3,  4,  4,  4,  4,  4,  5,  5,  5,
	  5,  6,  6,  6,  6,  7,  7,  7,  7,  8,  8,  8,  9,  9,  9, 10,
	 10, 10, 11, 11, 11, 12, 12, 13, 13, 13, 14, 14, 15, 15, 16, 16,
	 17, 17, 18, 18, 19, 19, 20, 20, 21, 21, 22, 22, 23, 24, 24, 25,
	 25, 26, 27, 27, 28, 29, 29, 30, 31, 32, 32, 33, 34, 35, 35, 36,
	 37, 38, 39, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 50,
	 51, 52, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 66, 67, 68,
	 69, 70, 72, 73, 74, 75, 77, 78, 79, 81, 82, 83, 85, 86, 87, 89,
	 90, 92, 93, 95, 96, 98, 99,101,102,104,105,107,109,110,112,114,
	115,117,119,120,122,124,126,127,129,131,133,135,137,138,140,142,
	144,146,148,150,152,154,156,158,160,162,164,167,169,171,173,175,
	177,180,182,184,186,189,191,193,196,198,200,203,205,208,210,213,
	215,218,220,223,225,228,231,233,236,239,241,244,247,249,252,255
};

local void enableSPIInterrupt() {
	SPCR |= _BV(SPIE);
}

local void disableSPIInterrupt() {
	SPCR &= ~_BV(SPIE);
}

local u8 red(u32 color) {
	return pgm_read_byte(&gamma8[color >> 16 & 0xFF]);
}

local u8 green(u32 color) {
	return pgm_read_byte(&gamma8[color >> 8 & 0xFF]);
}

local u8 blue(u32 color) {
	return pgm_read_byte(&gamma8[color >> 0 & 0xFF]);
}

local u8 brightness(u32 color) {
	return color >> 24 & 0x1F;
}

local void transmitNextByte() {
	switch (transmitState) {
		case IDLE:
			// This should never happen.
			break;

		case START_FRAME:
			SPDR = 0;

			if (++transmitIndex >= 8) {
				transmitState = LED_FRAME;
				transmitIndex = 0;
			}

			break;

		case LED_FRAME:
			switch (transmitIndex % 4) {
				case 0:
					SPDR = 0xE0 | brightness(ledColors[transmitIndex / 4]);
					break;
				case 1:
					SPDR = red(ledColors[transmitIndex / 4]);
					break;
				case 2:
					SPDR = blue(ledColors[transmitIndex / 4]);
					break;
				case 3:
					SPDR = green(ledColors[transmitIndex / 4]);
					break;
			}

			if (++transmitIndex > 4 * LED_COUNT) {
				transmitState = END_FRAME;
				transmitIndex = 0;
			}

			break;

		case END_FRAME:
			SPDR = 0xFF;

			if (++transmitIndex >= 8) {
				if (transmitAgain) {
					transmitState = START_FRAME;
					transmitIndex = 0;
					transmitAgain = false;
				} else {
					transmitState = IDLE;
					disableSPIInterrupt();
				}
			}

			break;
	}
}

void transmitLeds() {
	if (transmitState == IDLE) {
		transmitState = START_FRAME;
		transmitIndex = 0;
		enableSPIInterrupt();
		transmitNextByte();
	} else {
		transmitAgain = true;
	}
}

void setLed(u8 index, u32 color) {
	ledColors[index] = color;
}

void setAllLeds(u32 color) {
	for (u8 i = 0; i < LED_COUNT; ++i) {
		ledColors[i] = color;
	}
}

ISR(SPI_STC_vect) {
	transmitNextByte();
}
