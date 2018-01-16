#include "defs.h"
#include "leds.h"
#include "keys.h"
#include "tft.h"
#include "twi.h"
#include "usb.h"

void setup() {
	// Set clock prescaler to 1 regardless of fuse bits
	CLKPR = _BV(CLKPCE);
	CLKPR = 0;

	wdt_disable(); // This should be the default

	// Port B
	// 0-7: TFT D0-7 (normally output)
	// [ISP 1-3: SCK, MOSI, MISO]
	PORTB = 0;
	DDRB = 0xFF;

	// Port C
	// 6-7: Unused (floating)
	PORTC = _BV(6) | _BV(7);
	DDRC = 0;

	// Port D
	// 0:   SCL (ignored when TWEN bit in TWCR is set; external pull-up)
	// 1:   SDA (ignored when TWEN bit in TWCR is set; external pull-up)
	// 2-7: Unused (floating)
	PORTD = _BV(2) | _BV(3) | _BV(4) | _BV(5) | _BV(6) | _BV(7);
	DDRD = 0;

	// Port E
	// 2,6: Unused (floating)
	PORTE = _BV(2) | _BV(6);
	DDRE = 0;

	// Port F
	// 0:   Unused (floating)
	// 1:   TFT Backlight (output)
	// 4:   TFT RD (output, normally high)
	// 5:   TFT WR (output, normally high)
	// 6:   TFT C/D (output, normally high)
	// 7:   TFT CS (output, normally high)
	PORTF = _BV(0) | _BV(4) | _BV(5) | _BV(6) | _BV(7);
	DDRF = _BV(1) | _BV(4) | _BV(5) | _BV(6) | _BV(7);

	TWI_init();
	TFT_init();
	USB_init();
	enableGlobalInterrupts();

	// u32 tftid = TFT_readId();
	// while (USB_DEVICE_STATE != USB_DeviceState_Configured);
	// _delay_ms(10);
	// char str[16] = { 0 };
	// snprintf(str, sizeof(str), "0x%lx  ", tftid);
	// USB_debugSendString(str, sizeof(str));
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

const u16 colors[4] = { 0x0000, 0x07E0, 0xF800, 0x001F };

void loop() {
	requestKeys(TWI_ADDRESS_LEFT, &leftKeys);
	requestKeys(TWI_ADDRESS_RIGHT, &rightKeys);

	USB_update();

	updateLeds(&leftLeds, &leftKeys);

	transmitLeds(TWI_ADDRESS_LEFT, &leftLeds);
	transmitLeds(TWI_ADDRESS_RIGHT, &rightLeds);

	static u8 colorIndex = 0;
	TFT_fillScreen(colors[colorIndex++]);
	if (colorIndex >= 4) { colorIndex = 0; }
}

int main() {
	setup();

	while(1) {
		loop();
	}

	__builtin_unreachable();
}
