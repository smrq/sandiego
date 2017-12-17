#include "defs.h"
#include "led.h"
#include <util/delay.h>

local volatile enum TransmitState {
	START_FRAME,
	LED_FRAME,
	END_FRAME
} transmitState = START_FRAME;
local volatile int transmitIndex = 0;

local volatile uint8_t a = 0x01;
local volatile uint8_t r = 0xFF;
local volatile uint8_t g = 0x80;
local volatile uint8_t b = 0x00;

local uint32_t colors[] = {
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

local volatile bool heartbeat = false;

// local void enableSPIInterrupt() {
// 	SPCR |= _BV(SPIE);
// }

// local void disableSPIInterrupt() {
// 	SPCR &= ~_BV(SPIE);
// }

local void beat() {
	if (heartbeat) {
		PORTB &= ~_BV(0);
	} else {
		PORTB |= _BV(0);
	}
	heartbeat = !heartbeat;
}

// local void transmitNextByte() {
// 	switch (transmitState) {
// 		case START_FRAME:
// 			SPDR = 0;

// 			// if (transmitIndex == 0) {
// 				beat();
// 			// }

// 			if (++transmitIndex >= 8) {
// 				transmitState = LED_FRAME;
// 				transmitIndex = 0;
// 			}
// 			break;

// 		case LED_FRAME:
// 			switch (transmitIndex % 4) {
// 				case 0:
// 					SPDR = 0xE0 | a;
// 					break;
// 				case 1:
// 					SPDR = r;
// 					break;
// 				case 2:
// 					SPDR = b;
// 					break;
// 				case 3:
// 					SPDR = g;
// 					break;
// 			}

// 			if (++transmitIndex > 40) { // 10 LEDs for now
// 				transmitState = END_FRAME;
// 				transmitIndex = 0;
// 			}
// 			break;

// 		case END_FRAME:
// 			SPDR = 0xFF;

// 			if (++transmitIndex >= 8) {
// 				transmitState = START_FRAME;
// 				transmitIndex = 0;
// 				disableSPIInterrupt();
// 			}
// 			break;
// 	}
// }

void initializeLeds() {
	DDRB = _BV(PIN_SS) | _BV(PIN_MOSI) | _BV(PIN_SCK);
	PORTB &= ~(_BV(PIN_SS) | _BV(PIN_MOSI) | _BV(PIN_SCK));
	// SPCR = _BV(SPE) | _BV(MSTR) | _BV(CPOL) | _BV(CPHA) | _BV(SPR0);
	SPCR = _BV(SPE) | _BV(MSTR) | _BV(SPR0);
	SPSR &= ~_BV(SPI2X);

	// Heartbeat debugging
	DDRB |= _BV(0); // Output, pin 12 (PB0)
	PORTB &= ~_BV(0);
}

local void waitForTransfer() {
	while (!(SPSR & _BV(SPIF)));
}

void transmitLeds() {
	// enableSPIInterrupt();
	// transmitNextByte();

	for (int i = 0; i < 8; ++i) {
		SPDR = 0x00;
		waitForTransfer();
	}

	for (int i = 0; i < (sizeof(colors) / sizeof(colors[0])); ++i) {
		SPDR = 0xE0 | (colors[i] >> 24 & 0x1F);
		waitForTransfer();
		SPDR = colors[i] >> 16 & 0xFF;
		waitForTransfer();
		SPDR = colors[i] >> 0 & 0xFF;
		waitForTransfer();
		SPDR = colors[i] >> 8 & 0xFF;
		waitForTransfer();
	}

	for (int i = 0; i < 8; ++i) {
		SPDR = 0xFF;
		waitForTransfer();
	}

	beat();
	_delay_ms(10);
	beat();
	_delay_ms(10);
	beat();
	_delay_ms(10);
	beat();
	_delay_ms(10);
	beat();
	_delay_ms(10);
	beat();
}

// ISR(SPI_STC_vect) {
// 	transmitNextByte();
// }
