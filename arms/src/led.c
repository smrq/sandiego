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

local void enableSPIInterrupt() {
	SPCR |= _BV(SPIE);
}

local void disableSPIInterrupt() {
	SPCR &= ~_BV(SPIE);
}

local u8 red(u32 color) {
	return color >> 16 & 0xFF;
}

local u8 green(u32 color) {
	return color >> 8 & 0xFF;
}

local u8 blue(u32 color) {
	return color >> 0 & 0xFF;
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
