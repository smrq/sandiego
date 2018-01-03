#include "defs.h"
#include "heartbeat.h"

void beat() {
	static bool state = false;

	if (state) {
		PORTC &= ~_BV(7);
		state = false;
	} else {
		PORTC |= _BV(7);
		state = true;
	}
}
