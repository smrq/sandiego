#include "defs.h"
#include "heartbeat.h"
#include "led.h"

local u8 heartbeat = 0;

void beat() {
	PORTD = heartbeat++;
}

void debug(u8 output) {
	PORTD = output;
}
