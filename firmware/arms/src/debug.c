#include "defs.h"
#include "debug.h"

local volatile u8 state = 0;

local void debug_updateLeds() {
	if (state & _BV(0)) {
		PORTC &= ~_BV(7);
	} else {
		PORTC |= _BV(7);
	}

	if (state & _BV(1)) {
		PORTB &= ~_BV(6);
	} else {
		PORTB |= _BV(6);
	}

	if (state & _BV(2)) {
		PORTB &= ~_BV(7);
	} else {
		PORTB |= _BV(7);
	}
}

void debug_reset() {
	state = 0;
	debug_updateLeds();
}

void debug_count() {
	state = (state + 1) % (1 << 3);
	debug_updateLeds();
}

void debug_output(u8 value) {
	state = value;
	debug_updateLeds();
}
