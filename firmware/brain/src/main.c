#include "defs.h"
#include "twi.h"
#include <util/delay.h>

void setup() {
	DDRD = _BV(6);
	PORTD &= ~(_BV(6));

	TWI_init();
	enableGlobalInterrupts();
}

void setLed(u8 address, u8 index, u32 color) {
	struct PACKED {
		u8 command;
		u8 index;
		u32 color;
	} message = {
		.command = TWI_CMD_SET_LED,
		.index = index,
		.color = color
	};

	TWI_transmit(address, (u8 *)&message, sizeof(message));
}

void setAllLeds(u8 address, u32 color) {
	struct PACKED {
		u8 command;
		u32 color;
	} message = {
		.command = TWI_CMD_SET_ALL_LEDS,
		.color = color
	};

	TWI_transmit(address, (u8 *)&message, sizeof(message));
}

void setLedPattern(u8 address, u32 *colors) {
	for (u8 bank = 0; bank < LED_BANKS; ++bank) {
		struct PACKED {
			u8 command;
			bool flush;
			u8 bank;
			u32 colors[LED_BANK_SIZE];
		} message = {
			.command = TWI_CMD_SET_LED_BANK,
			.flush = bank == LED_BANKS - 1,
			.bank = bank
		};

		u8 i;

		for (i = 0;
			(i < LED_BANK_SIZE) && ((bank * LED_BANK_SIZE) + i < LED_COUNT);
			++i
		) {
			message.colors[i] = colors[(bank * LED_BANK_SIZE) + i];
		}

		TWI_transmit(
			address,
			(u8 *)&message,
			sizeof(message) - (sizeof(message.colors[0]) * (LED_BANK_SIZE - i))
		);

		while (TWI_busy());
	}
}

void nextLed() {
	u8 address = TWI_BASE_ADDRESS;
	static u8 index = 0;

	if (index < 1) {
		setAllLeds(address, 0x1000000);
	}
	else if (index < 11) {
		setLed(address, index - 1, 0x1FF0000);
	}
	else if (index < 21) {
		setLed(address, index - 11, 0x100FF00);
	}
	else if (index < 31) {
		setLed(address, index - 21, 0x10000FF);
	}
	else {
		if (index % 2) {
			u32 colors[LED_COUNT] = {
				0x1FF0000, 0x100FFFF, 0x1FF0000, 0x100FFFF, 0x1FF0000, 0x100FFFF, 0x1FF0000, 0x100FFFF, 0x1FF0000, 0x100FFFF
			};
			setLedPattern(address, colors);
		} else {
			u32 colors[LED_COUNT] = {
				0x100FFFF, 0x1FF0000, 0x100FFFF, 0x1FF0000, 0x100FFFF, 0x1FF0000, 0x100FFFF, 0x1FF0000, 0x100FFFF, 0x1FF0000
			};
			setLedPattern(address, colors);
		}
	}

	index = (index + 1) % 48;
}

void loop() {
	nextLed();
	_delay_ms(100);
}

int main() {
	setup();

	while(1) {
		// loop();
	}
	__builtin_unreachable();
}
