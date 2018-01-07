#include "defs.h"
#include "leds.h"
#include "twi.h"
#include <util/delay.h>

volatile bool shouldRequestKeys = false;

void setup() {
	// Port B
	// 0:   SS (unused -- floating)
	// 1-3: SCK, MOSI, MISO (floating -- connected only when programming with ISP)
	// 4:   Debug LED (output)
	// 5-7: Unused (floating)
	PORTB = ~_BV(4); //0xFF;
	DDRB = _BV(4);

	// Port C
	// 6-7: Unused (floating)
	PORTC = _BV(6) | _BV(7);
	DDRC = 0;

	// Port D
	// 0:   SCL (ignored when TWEN bit in TWCR is set; external pull-up)
	// 1:   SDA (ignored when TWEN bit in TWCR is set; external pull-up)
	// 2:   INT2 (callback interrupt -- input, internal pullup)
	// 3-6: Unused (floating)
	PORTD = ~(_BV(0) | _BV(1));
	DDRD = 0;

	// Port E
	// 2,6: Unused (floating)
	PORTE = _BV(2) | _BV(6);
	DDRE = 0;

	// Port F
	// 0-1: Unused (floating)
	// 4-7: Unused (floating)
	PORTF = ~(_BV(2) | _BV(3));
	DDRF = 0;

	// External interrupts
	EICRA = _BV(5); // Trigger INT2 on falling edge (datasheet p. 89)
	EIMSK = _BV(2); // Enable INT2

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
				0x1FF0000, 0x100FFFF, 0x1FF0000, 0x100FFFF, 0x1FF0000,
				0x100FFFF, 0x1FF0000, 0x100FFFF, 0x1FF0000, 0x100FFFF
			};
			writeLedPattern(buffer, colors);
		} else {
			u32 colors[LED_COUNT] = {
				0x100FFFF, 0x1FF0000, 0x100FFFF, 0x1FF0000, 0x100FFFF,
				0x1FF0000, 0x100FFFF, 0x1FF0000, 0x100FFFF, 0x1FF0000
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

	while (!TWI_transmit(address, (u8 *)&message, sizeof(message)));
}

void requestKeys() {
	// TODO: perform request

	shouldRequestKeys = false;
	PORTB |= _BV(4);
}

void loop() {
	nextLed(&leftLeds);
	transmitLeds(&leftLeds, TWI_BASE_ADDRESS);
	if (shouldRequestKeys) {
		requestKeys();
	}
	_delay_ms(500);
}

int main() {
	setup();

	while(1) {
		loop();
	}
	__builtin_unreachable();
}

ISR(INT2_vect) {
	shouldRequestKeys = true;
	PORTB &= ~_BV(4);
}
