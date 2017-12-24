#include "defs.h"
#include "heartbeat.h"
#include "led.h"

local u8 heartbeat = 0;

void beat() {
	PORTD = ++heartbeat;
}

void beat_reset() {
	PORTD = heartbeat = 0;
}

void debug(u8 output) {
	PORTD = output;
}
