#include "defs.h"
#include "leds.h"
#include "twi.h"
#include <util/delay.h>

void setup() {
	DDRD = _BV(6);
	PORTD &= ~(_BV(6));

	TWI_init();
	enableGlobalInterrupts();
}

void nextLed(led_buffer_t *buffer) {
	static u8 index = 0;

	if (index < 1) {
		writeAllLeds(buffer, 0x1000000);
	}
	else if (index < 11) {
		writeLed(buffer, index - 1, 0x1FF0000);
	}
	else if (index < 21) {
		writeLed(buffer, index - 11, 0x100FF00);
	}
	else if (index < 31) {
		writeLed(buffer, index - 21, 0x10000FF);
	}
	else {
		if (index % 2) {
			u32 colors[LED_COUNT] = {
				0x1FF0000, 0x100FFFF, 0x1FF0000, 0x100FFFF, 0x1FF0000, 0x100FFFF, 0x1FF0000, 0x100FFFF, 0x1FF0000, 0x100FFFF
			};
			writeLedPattern(buffer, colors);
		} else {
			u32 colors[LED_COUNT] = {
				0x100FFFF, 0x1FF0000, 0x100FFFF, 0x1FF0000, 0x100FFFF, 0x1FF0000, 0x100FFFF, 0x1FF0000, 0x100FFFF, 0x1FF0000
			};
			writeLedPattern(buffer, colors);
		}
	}

	index = (index + 1) % 48;
}

void transmitLeds(led_buffer_t *buffer, u8 address) {
	flipLedBuffer(buffer);

	struct PACKED {
		u8 command;
		u32 colors[LED_COUNT];
	} message = {
		.command = TWI_CMD_SET_LEDS
	};

	for (u8 index = 0; index < LED_COUNT; ++index) {
		message.colors[index] = readLed(buffer, index);
	}

	TWI_transmit(address, (u8 *)&message, sizeof(message));
}

void loop() {
	nextLed(&leftLeds);
	transmitLeds(&leftLeds, TWI_BASE_ADDRESS);
	_delay_ms(100);
}

int main() {
	setup();

	while(1) {
		loop();
	}
	__builtin_unreachable();
}
