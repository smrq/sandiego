#include <util/delay.h>
#include "defs.h"
#include "led.h"

local void setup() {
	enableGlobalInterrupts();
	initializeLeds();
	transmitLeds();
}

local void loop() {
	// transmitLeds();
	_delay_ms(1000);
}

int main(void) {
	setup();

	while (1) {
		loop();
	}
	__builtin_unreachable();
}
