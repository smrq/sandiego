#include <avr/cpufunc.h>
#include "defs.h"
#include "keyscanner.h"

local keyscanner_rowstate_t rowState[ROW_COUNT] = { 0 };

local u8 keyscanner_debounce(keyscanner_rowstate_t *row, u8 pressed) {
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

void keyscanner_scan() {
	for (u8 row = 0; row < ROW_COUNT; ++row) {
		// Note: In order for these bit operations to be atomic, the RHS of the
		// assignment has to be a compile-time constant. This enables the compiler
		// to generate SBI/CBI instructions. This is necessary to prevent race
		// conditions with interrupt handlers operating on the same I/O ports.
		switch (row) {
#if ROW_COUNT > 0
			case 0: PORTC &= ~_BV(0); break;
#endif
#if ROW_COUNT > 1
			case 1: PORTC &= ~_BV(1); break;
#endif
#if ROW_COUNT > 2
			case 2: PORTC &= ~_BV(2); break;
#endif
#if ROW_COUNT > 3
			case 3: PORTC &= ~_BV(3); break;
#endif
#if ROW_COUNT > 4
			case 4: PORTA &= ~_BV(0); break;
#endif
#if ROW_COUNT > 5
			case 5: PORTA &= ~_BV(1); break;
#endif
#if ROW_COUNT > 6
			case 6: PORTA &= ~_BV(2); break;
#endif
#if ROW_COUNT > 7
			case 7: PORTA &= ~_BV(3); break;
#endif
		}

		/*
			When reading back a software assigned pin value, a nop instruction must be
			inserted as indicated in Figure 10-5.
				- ATtiny datasheet, p. 63
		*/
		_NOP();

		u8 updatedKeys = keyscanner_debounce(&rowState[row], ~PIND);
		if (__builtin_expect(updatedKeys, 0)) {
			// TODO: Do something!
		}

		switch (row) {
#if ROW_COUNT > 0
			case 0: PORTC |= _BV(0); break;
#endif
#if ROW_COUNT > 1
			case 1: PORTC |= _BV(1); break;
#endif
#if ROW_COUNT > 2
			case 2: PORTC |= _BV(2); break;
#endif
#if ROW_COUNT > 3
			case 3: PORTC |= _BV(3); break;
#endif
#if ROW_COUNT > 4
			case 4: PORTA |= _BV(0); break;
#endif
#if ROW_COUNT > 5
			case 5: PORTA |= _BV(1); break;
#endif
#if ROW_COUNT > 6
			case 6: PORTA |= _BV(2); break;
#endif
#if ROW_COUNT > 7
			case 7: PORTA |= _BV(3); break;
#endif
		}
	}
}

u8 keyscanner_getRowState(u8 row) {
	return rowState[row].current;
}
