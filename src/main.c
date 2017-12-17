#include <util/delay.h>
#include "defs.h"
#include "heartbeat.h"
#include "led.h"

local void setup() {
	// SPI
	DDRB = _BV(PIN_SS) | _BV(PIN_MOSI) | _BV(PIN_SCK);
	PORTB &= ~(_BV(PIN_SS) | _BV(PIN_MOSI) | _BV(PIN_SCK));
	SPCR = _BV(SPE) | _BV(MSTR) | _BV(CPOL) | _BV(CPHA) | _BV(SPR0);
	SPSR &= ~_BV(SPI2X);

	// LED Heartbeat
	DDRD = _BV(0);
	PORTD &= ~_BV(0);

	enableGlobalInterrupts();
}

local void loop() {
	static u8 index = 0;

	u32 color;

	switch (index % 6) {
		case 0:
			color = 0x04FF0000;
			break;
		case 1:
			color = 0x0400FF00;
			break;
		case 2:
			color = 0x040000FF;
			break;
		case 3:
			color = 0x04FFFF00;
			break;
		case 4:
			color = 0x04FF00FF;
			break;
		case 5:
			color = 0x0400FFFF;
			break;
	}

	beat();
	setLedColor((index++) % LED_COUNT, color);

	transmitLeds();
	_delay_ms(50);
}

int main(void) {
	setup();

	while (1) {
		loop();
	}
	__builtin_unreachable();
}
