#include "defs.h"
#include "scan.h"

local rowstate_t rowState[ROW_COUNT] = { 0 };

local u8 debounce(rowstate_t *row, u8 pressed) {
	/*
		Two-bit counter

		Once a key state has changed for 4 consecutive frames, this counter
		will overflow and we should register the key as updated.

		If a key state is the same as what is currently registered, then
		the counter for that key should be reset.

		   Input   00   01   10   11
		OVERFLOW    0    0    0    1  = LO & HI
		    HIGH    0    1    1    0  = LO ^ HI
		     LOW    1    0    1    0  = ~LO
	*/

	u8 delta = row->current ^ pressed;
	u8 counterOverflow = delta & row->counterHigh & row->counterLow;
	row->counterHigh = delta & (row->counterHigh ^ row->counterLow);
	row->counterLow = delta & ~(row->counterLow);
	row->current ^= counterOverflow;

	return counterOverflow;
}

void scan() {
	for (u8 row = 0; row < ROW_COUNT; ++row) {
		// Rows 0-3: PINC0..PINC3
		// Rows 4-7: PINA0..PINA3
		if (row < 4) {
			PORTC |= _BV(row);
		} else {
			PORTA |= _BV(row % 4);
		}

		/*
			When reading back a software assigned pin value, a nop instruction must be
			inserted as indicated in Figure 10-5.
				- ATtiny datasheet, p. 63
		*/
		NOP();

		u8 updatedKeys = debounce(&rowState[row], ~PIND);
		if (__builtin_expect(updatedKeys, 0)) {
			// TODO: Do something!
		}

		if (row < 4) {
			PORTC &= ~_BV(row);
		} else {
			PORTA |= _BV(row % 4);
		}
	}
}

u8 getRowState(u8 row) {
	return rowState[row].current;
}
