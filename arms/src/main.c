#include <util/delay.h>
#include "defs.h"
#include "heartbeat.h"
#include "led.h"
#include "twi.h"

local void handleTWIReceivedData(u8 *src, __attribute__ ((unused)) u8 length) {
	switch (src[0]) {
		case 0x73: // Set single LED. Completely arbitrary test value
			setLedColor(src[1], *(u32 *)(src + 2));
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
	DDRD = _BV(0);
	PORTD &= ~_BV(0);

	// TWI
	TWI_receivedData = handleTWIReceivedData;
	// PINB0 and PINB1 are used to give a local address to this component
	// Thus, up to four of these components on a bus can be individually addressed
	TWI_init(TWI_BASE_ADDRESS | (PINB & 0x03), false);

	enableGlobalInterrupts();
}

local void loop() {
	// static u8 index = 0;

	// u32 color;

	// switch (index % 6) {
	// 	case 0:
	// 		color = 0x04FF0000;
	// 		break;
	// 	case 1:
	// 		color = 0x0400FF00;
	// 		break;
	// 	case 2:
	// 		color = 0x040000FF;
	// 		break;
	// 	case 3:
	// 		color = 0x04FFFF00;
	// 		break;
	// 	case 4:
	// 		color = 0x04FF00FF;
	// 		break;
	// 	case 5:
	// 		color = 0x0400FFFF;
	// 		break;
	// }

	// beat();
	// setLedColor((index++) % LED_COUNT, color);

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
