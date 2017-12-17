#include "heartbeat.h"
#include "defs.h"
#include "led.h"

local bool heartbeat = false;

void beat() {
	if (heartbeat) {
		PORTD &= ~_BV(0);
	} else {
		PORTD |= _BV(0);
	}
	heartbeat = !heartbeat;
}
