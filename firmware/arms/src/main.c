#include "defs.h"
#include "heartbeat.h"
#include "led.h"
#include "main.h"
#include "scan.h"
#include "twi.h"

void TWI_requestedData(UNUSED u8 *dest, UNUSED u8 *length) { }

local void setup() {
	// Row pins are configured as outputs that are normally high.
	// Column pins are configured as inputs with the pull-up resistor enabled.
	// When doing a key scan, a single row is set to low.
	// When a keyswitch is pressed, then the corresponding column will be driven low.
	// Otherwise, each column will be pulled high by the internal pull-up resistor.

	// Port A
	// 0:   Row 4 (output, normally high)
	// 1-3: Rows 5-7 (floating -- currently unused)
	PORTA = 0b1111;
	DDRA =  0b0001;

	// Port B
	// 0-1: Address jumpers (either floating or low)
	// 2:   SS (unused, floating)
	// 3:   MOSI (output)
	// 4:   MISO (floating -- connected only when programming with ISP)
	// 5:   SCK (output)
	// 6-7: Unused (floating)
	PORTB = 0b11010111;
	DDRB =  0b00101000;

	// Port C
	// 0-3: Rows 0-3 (output, normally high)
	// 4:   SDA (ignored when TWEN bit in TWCR is set; external pull-up)
	// 5:   SCL (ignored when TWEN bit in TWCR is set; external pull-up)
	// 6:   RESET (ignored when RSTDISBL fuse is set; external pull-up)
	// 7:   Heartbeat LED (output)
	PORTC = 0b00001111;
	DDRC =  0b10001111;

	// Port D
	// 0-7: Columns 0-7 (input, internal pullup)
	PORTD = 0b11111111;
	DDRD =  0b00000000;

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
	scan();
}

int main() {
	setup();

	while (1) {
		loop();
	}

	__builtin_unreachable();
}
