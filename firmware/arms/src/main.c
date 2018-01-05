#include "defs.h"
#include "heartbeat.h"
#include "led.h"
#include "main.h"
#include "scan.h"
#include "twi.h"

local void setup() {
	// Row pins are configured as outputs that are normally high.
	// Column pins are configured as inputs with the pull-up resistor enabled.
	// When doing a key scan, a single row is set to low.
	// When a keyswitch is pressed, then the corresponding column will be driven low.
	// Otherwise, each column will be pulled high by the internal pull-up resistor.

	// Port A
	// 0-3: Rows 4-7 (if used: output, normally high; if unused: floating)
	PORTA = 0b1111;
	DDRA =
		(ROW_COUNT >= 5 ? _BV(0) : 0) |
		(ROW_COUNT >= 6 ? _BV(1) : 0) |
		(ROW_COUNT >= 7 ? _BV(2) : 0) |
		(ROW_COUNT >= 8 ? _BV(3) : 0);

	// Port B
	// 0-1: Address jumpers (either floating or low)
	// 2:   SS (unused, floating)
	// 3:   MOSI (output)
	// 4:   MISO (floating -- connected only when programming with ISP)
	// 5:   SCK (output)
	// 6-7: Unused (floating)
	PORTB = _BV(0) | _BV(1) | _BV(2) | _BV(4) | _BV(6) | _BV(7);
	DDRB = _BV(3) | _BV(5);

	// Port C
	// 0-3: Rows 0-3 (if used: output, normally high; if unused: floating)
	// 4:   SDA (ignored when TWEN bit in TWCR is set; external pull-up)
	// 5:   SCL (ignored when TWEN bit in TWCR is set; external pull-up)
	// 6:   RESET (ignored when RSTDISBL fuse is set; external pull-up)
	// 7:   Heartbeat LED (output)
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
	/*
	// Keyscanner debugging using LEDs
	setAllLeds(0x1000000);
	u32 colors[8] = { 0x1202020, 0x1202020, 0x1202020, 0x1202020, 0x1202020, 0x1202020, 0x1202020, 0x1202020 };
	static u8 flashing = 0;
	for (u8 i = 0; i < ROW_COUNT; ++i) {
		u8 rowState = getRowState(i);
		for (u8 bit = 0; bit < 8; ++bit) {
			if (rowState & _BV(bit)) {
				colors[bit] |=
					(i == 0 ? 0xFF0000 :
					i == 1 ? 0x00FF00 :
					i == 2 ? 0x0000FF :
					i == 3 ? 0x8000000 :
					(i == 4 && (++flashing & 0x80)) ? 0xFFFFFF :
					0);
			}
		}
	}
	for (u8 i = 0; i < 8; ++i) {
		setLed(i, colors[i]);
	}
	transmitLeds();
	// END
	*/
}

int main() {
	setup();

	while (1) {
		loop();
	}

	__builtin_unreachable();
}
