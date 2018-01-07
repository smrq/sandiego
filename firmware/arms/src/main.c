#include "defs.h"
#include "keyscanner.h"
#include "spi.h"
#include "twi.h"

local void setup() {
	// Row pins are configured as outputs that are normally high.
	// Column pins are configured as inputs with the pull-up resistor enabled.
	// When doing a key scan, a single row is set to low.
	// When a keyswitch is pressed, then the corresponding column will be driven low.
	// Otherwise, each column will be pulled high by the internal pull-up resistor.

	// Port A
	// 0-3: Rows 4-7 (if used: output, normally high; if unused: floating)
	PORTA = 0xF;
	DDRA =
		(ROW_COUNT >= 5 ? _BV(0) : 0) |
		(ROW_COUNT >= 6 ? _BV(1) : 0) |
		(ROW_COUNT >= 7 ? _BV(2) : 0) |
		(ROW_COUNT >= 8 ? _BV(3) : 0);

	// Port B
	// 0-1: TWI address jumpers (either floating or low)
	// 2:   SS (unused, floating)
	// 3:   MOSI (output)
	// 4:   MISO (floating -- connected only when programming with ISP)
	// 5:   SCK (output)
	// 6-7: Debug LEDs (output)
	PORTB = _BV(0) | _BV(1) | _BV(2) | _BV(4);
	DDRB = _BV(3) | _BV(5) | _BV(6) | _BV(7);

	// Port C
	// 0-3: Rows 0-3 (if used: output, normally high; if unused: floating)
	// 4:   SDA (ignored when TWEN bit in TWCR is set; external pull-up)
	// 5:   SCL (ignored when TWEN bit in TWCR is set; external pull-up)
	// 6:   RESET (ignored when RSTDISBL fuse is set; external pull-up)
	// 7:   Debug LED (output)
	PORTC = _BV(0) | _BV(1) | _BV(2) | _BV(3);
	DDRC =
		(ROW_COUNT >= 1 ? _BV(0) : 0) |
		(ROW_COUNT >= 2 ? _BV(1) : 0) |
		(ROW_COUNT >= 3 ? _BV(2) : 0) |
		(ROW_COUNT >= 4 ? _BV(3) : 0) |
		_BV(7);

	// Port D
	// 0-7: Columns 0-7 (input, internal pullup)
	PORTD = 0xFF;
	DDRD = 0x00;

	SPI_init();
	TWI_init(TWI_BASE_ADDRESS | (PINB & 0x03), false);
	enableGlobalInterrupts();

	SPI_transmit();
}

local void loop() {
	keyscanner_scan();
}

int main() {
	setup();

	while (1) {
		loop();
	}

	__builtin_unreachable();
}
