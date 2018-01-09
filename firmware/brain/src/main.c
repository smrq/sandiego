#include "defs.h"
#include "debug.h"
#include "leds.h"
#include "keys.h"
#include "twi.h"
#include "usb.h"

void setup() {
	// Set clock prescaler to 1 regardless of fuse bits
	CLKPR = _BV(CLKPCE);
	CLKPR = 0;

	wdt_disable(); // This should be the default

	// Port B
	// 0:   SS (unused -- floating)
	// 1-3: SCK, MOSI, MISO (floating -- connected only when programming with ISP)
	// 4-7: Debug LED (output)
	PORTB = _BV(0) | _BV(1) | _BV(2) | _BV(3) | _BV(4) | _BV(5) | _BV(6) | _BV(7);
	DDRB = _BV(4) | _BV(5) | _BV(6) | _BV(7);

	// Port C
	// 6-7: Unused (floating)
	PORTC = _BV(6) | _BV(7);
	DDRC = 0;

	// Port D
	// 0:   SCL (ignored when TWEN bit in TWCR is set; external pull-up)
	// 1:   SDA (ignored when TWEN bit in TWCR is set; external pull-up)
	// 2-6: Unused (floating)
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

	TWI_init();
	USB_init();
	enableGlobalInterrupts();
}

void updateLeds(led_buffer_t *leds, key_buffer_t *keys) {
	for (u8 col = 0; col < COL_COUNT; ++col) {
		u32 color = 0x1000000;
		if (keys->front[0] & _BV(col)) {
			color |= 0xFF0000;
		}
		if (keys->front[1] & _BV(col)) {
			color |= 0x00FF00;
		}
		if (keys->front[2] & _BV(col)) {
			color |= 0x0000FF;
		}
		if (keys->front[3] & _BV(col)) {
			color |= 0xFFFF00;
		}
		if (keys->front[4] & _BV(col)) {
			color |= 0xFF00FF;
		}
		leds->back[col] = color;
	}
}

void loop() {
	requestKeys(TWI_ADDRESS_LEFT, &leftKeys);
	requestKeys(TWI_ADDRESS_RIGHT, &rightKeys);

	USB_update();

	updateLeds(&leftLeds, &leftKeys);

	transmitLeds(TWI_ADDRESS_LEFT, &leftLeds);
	transmitLeds(TWI_ADDRESS_RIGHT, &rightLeds);
}

int main() {
	setup();

	while(1) {
		loop();
	}

	__builtin_unreachable();
}
