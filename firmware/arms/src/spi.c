#include "defs.h"
#include "debug.h"
#include "leds.h"
#include "spi.h"

local volatile enum TransmitState {
	IDLE,
	START_FRAME,
	LED_FRAME_A,
	LED_FRAME_R,
	LED_FRAME_B,
	LED_FRAME_G,
	END_FRAME,
	FINISHED
} transmitState = IDLE;
local volatile bool transmitAgain = false;
local volatile u8 transmitIndex = 0;

local void SPI_sendByte() {
	switch (transmitState) {
		case IDLE:
			// This should never happen.
			break;

		case START_FRAME:
			SPDR = 0;
			if (++transmitIndex == 8) {
				transmitState = LED_FRAME_A;
				transmitIndex = 0;
			}
			break;

		case LED_FRAME_A:
			SPDR = 0xE0 | led_getBrightnessValue(transmitIndex);
			transmitState = LED_FRAME_R;
			break;

		case LED_FRAME_R:
			SPDR = led_getRedValue(transmitIndex);
			transmitState = LED_FRAME_B;
			break;

		case LED_FRAME_B:
			SPDR = led_getBlueValue(transmitIndex);
			transmitState = LED_FRAME_G;
			break;

		case LED_FRAME_G:
			SPDR = led_getGreenValue(transmitIndex);
			if (++transmitIndex == LED_COUNT) {
				transmitState = END_FRAME;
				transmitIndex = 0;
			} else {
				transmitState = LED_FRAME_A;
			}
			break;

		case END_FRAME:
			SPDR = 0xFF;
			if (++transmitIndex == 8) {
				if (transmitAgain) {
					transmitState = START_FRAME;
					transmitIndex = 0;
					transmitAgain = false;
				} else {
					transmitState = FINISHED;
				}
			}
			break;

		case FINISHED:
			transmitState = IDLE;
			break;
	}
}

void SPI_init() {
	SPSR = SPSR_CLOCK_SETTINGS;
	SPCR = _BV(SPIE) | _BV(SPE) | _BV(MSTR) | _BV(CPOL) | _BV(CPHA) | SPCR_CLOCK_SETTINGS;
}

void SPI_transmit() {
	if (transmitState == IDLE) {
		transmitState = START_FRAME;
		transmitIndex = 0;
		SPI_sendByte();
	} else {
		transmitAgain = true;
	}
}

ISR(SPI_STC_vect) {
	SPI_sendByte();
}
