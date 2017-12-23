#include <util/delay.h>
#include "defs.h"
#include "heartbeat.h"
#include "led.h"
#include "main.h"
#include "twi.h"

void TWI_requestedData(u8 *dest, u8 *length) {}
void TWI_receivedData(u8 *src, __attribute__ ((unused)) u8 length) {
	// debug(length);

	twiCommand_t *command = (twiCommand_t *)src;

	switch (command->type) {
		case TWI_CMD_SET_LED:
			setLed(command->data.setLed.index, command->data.setLed.color);
			transmitLeds();
			break;
		case TWI_CMD_SET_ALL_LEDS:
			setAllLeds(command->data.setAllLeds.color);
			transmitLeds();
			break;
		case TWI_CMD_SET_LED_PATTERN:
			setLedPattern(command->data.setLedPattern.colors);
			transmitLeds();
			break;
	}
}

local void setup() {
	// SPI
	DDRB = _BV(PIN_SS) | _BV(PIN_MOSI) | _BV(PIN_SCK);
	PORTB &= ~(_BV(PIN_SS) | _BV(PIN_MOSI) | _BV(PIN_SCK));
	SPCR = _BV(SPE) | _BV(MSTR) | _BV(CPOL) | _BV(CPHA) | _BV(SPR0);
	SPSR &= ~_BV(SPI2X);

	// LED Heartbeat
	DDRD = 0xFF;
	PORTD = 0;

	// TWI
	// PINB0 and PINB1 are used to give a local address to this component
	// Thus, up to four of these components on a bus can be individually addressed
	TWI_init(TWI_BASE_ADDRESS | (PINB & 0x03), false);

	enableGlobalInterrupts();

	transmitLeds();
}

local void loop() {
	// Poll keyswitches
}

int main(void) {
	setup();

	while (1) {
		loop();
	}
	__builtin_unreachable();
}
