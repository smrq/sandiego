#include "defs.h"
#include "led.h"

local volatile enum TransmitState {
	START_FRAME,
	LED_FRAME,
	END_FRAME
} transmitState = START_FRAME;
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
		case START_FRAME: {
			SPDR = 0;

			if (++transmitIndex >= 8) {
				transmitState = LED_FRAME;
				transmitIndex = 0;
			}
		} break;

		case LED_FRAME: {
			u16 ledIndex = transmitIndex / 4;

			switch (transmitIndex % 4) {
				case 0:
					SPDR = 0xE0 | brightness(ledColors[ledIndex]);
					break;
				case 1:
					SPDR = red(ledColors[ledIndex]);
					break;
				case 2:
					SPDR = blue(ledColors[ledIndex]);
					break;
				case 3:
					SPDR = green(ledColors[ledIndex]);
					break;
			}

			if (++transmitIndex > 4 * LED_COUNT)  {
				transmitState = END_FRAME;
				transmitIndex = 0;
			}
		} break;

		case END_FRAME: {
			SPDR = 0xFF;

			if (++transmitIndex >= 8) {
				transmitState = START_FRAME;
				transmitIndex = 0;
				disableSPIInterrupt();
			}
		} break;
	}
}

void transmitLeds() {
	enableSPIInterrupt();
	transmitNextByte();
}

void setLedColor(u8 index, u32 color) {
	disableGlobalInterrupts();
	ledColors[index] = color;
	enableGlobalInterrupts();
}

void setAllLedColors(u32 *colors) {
	disableGlobalInterrupts();
	for (u16 i = 0; i < LED_COUNT; ++i) {
		ledColors[i] = colors[i];
	}
	enableGlobalInterrupts();
}

ISR(SPI_STC_vect) {
	transmitNextByte();
}
