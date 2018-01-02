#include "defs.h"
#include "led.h"
#include "main.h"
#include "twi.h"

void TWI_requestedData(
	__attribute__ ((unused)) u8 *dest,
	__attribute__ ((unused)) u8 *length) { }

local void setup() {

	DDRB = _BV(PIN_SS) | _BV(PIN_MOSI) | _BV(PIN_SCK);
	PORTB &= ~(_BV(PIN_SS) | _BV(PIN_MOSI) | _BV(PIN_SCK));
	SPCR = _BV(SPE) | _BV(MSTR) | _BV(CPOL) | _BV(CPHA) | SPCR_CLOCK_SETTINGS;
	SPSR = SPSR_CLOCK_SETTINGS;

	/*
		TWI
		PINB0 and PINB1 are used to give a local address to this component.
		Thus, up to four of these components on a bus can be individually addressed.
	*/
	TWI_init(TWI_BASE_ADDRESS | (PINB & 0x03), false);

	enableGlobalInterrupts();

	transmitLeds();
}

local void loop() {
	// Poll keyswitches
}

int main() {
	setup();

	while (1) {
		loop();
	}
	__builtin_unreachable();
}
