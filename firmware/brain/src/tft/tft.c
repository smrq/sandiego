#include "defs.h"
#include "./tft.h"

local i16 TFT_Width;
local i16 TFT_Height;
local u8 TFT_Rotation;

local void TFT_setActive()   { PORTF &= ~_BV(7); }
local void TFT_clearActive() { PORTF |=  _BV(7); }
local void TFT_setCommand()  { PORTF &= ~_BV(6); }
local void TFT_setData()     { PORTF |=  _BV(6); }
local void TFT_setWrite()    { PORTF &= ~_BV(5); }
local void TFT_clearWrite()  { PORTF |=  _BV(5); }
local void TFT_setRead()     { PORTF &= ~_BV(4); }
local void TFT_clearRead()   { PORTF |=  _BV(4); }

local void TFT_resend() {
	TFT_setWrite();
	TFT_clearWrite();
}

void TFT_enableBacklight() {
	PORTF |= _BV(1);
}

void TFT_disableBacklight() {
	PORTF &= ~_BV(1);
}

local void TFT_setReadDirection() {
	DDRB = 0;
	PORTB = 0;
}

local void TFT_setWriteDirection() {
	DDRB = 0xFF;
	PORTB = 0;
}

local void TFT_write8(u8 value) {
	TFT_setWrite();
	PORTB = value;
	TFT_clearWrite();
}

local void TFT_write16(u16 value) {
	TFT_write8((value >> 8) & 0xFF);
	TFT_write8((value >> 0) & 0xFF);
}

local void TFT_write24(u32 value) {
	TFT_write8((value >> 16) & 0xFF);
	TFT_write8((value >>  8) & 0xFF);
	TFT_write8((value >>  0) & 0xFF);
}

local void TFT_write32(u32 value) {
	TFT_write8((value >> 24) & 0xFF);
	TFT_write8((value >> 16) & 0xFF);
	TFT_write8((value >>  8) & 0xFF);
	TFT_write8((value >>  0) & 0xFF);
}

local void TFT_writeCommand(u8 command) {
	TFT_setCommand();
	TFT_write8(command);
	TFT_setData();
}

local u8 TFT_read8() {
	TFT_setRead();

	// Pixel read operations require a minimum 400 nS delay from RD active
	// to polling the input pins.  At 16 MHz, one machine cycle is 62.5 nS.
	__builtin_avr_delay_cycles(7);

	u8 result = (PIND & 0b11000000) | ((PINF & 0b11110000) >> 2) | (PINF & 0b00000011);
	TFT_clearRead();
	return result;
}

local u16 TFT_read16() {
	u16 result = TFT_read8();
	result <<= 8;
	result |= TFT_read8();
	return result;
}

local u32 TFT_read24() {
	u32 result = TFT_read8();
	result <<= 8;
	result |= TFT_read8();
	result <<= 8;
	result |= TFT_read8();
	return result;
}

local u32 TFT_read32() {
	u32 result = TFT_read8();
	result <<= 8;
	result |= TFT_read8();
	result <<= 8;
	result |= TFT_read8();
	result <<= 8;
	result |= TFT_read8();
	return result;
}

local void TFT_setAddr(u16 x0, u16 y0, u16 x1, u16 y1) {
	TFT_writeCommand(ILI9341_COLUMN_ADDRESS_SET);
	TFT_write16(x0);
	TFT_write16(x1);
	TFT_writeCommand(ILI9341_PAGE_ADDRESS_SET);
	TFT_write16(y0);
	TFT_write16(y1);
}

void TFT_init() {
	TFT_Width = TFT_HEIGHT;
	TFT_Height = TFT_WIDTH;
	TFT_Rotation = ILI9341_MAC_ROTATION_90;

    /*
    	It is necessary to wait 5msec after releasing RESX before sending commands.
    		- ILI9341 datasheet v1.02, section 15.4. Reset Timing, p. 218
    */
    _delay_ms(5);

	/* https://github.com/adafruit/TFTLCD-Library/blob/d73bbf18/Adafruit_TFTLCD.cpp#L279 */
	TFT_setActive();
	TFT_writeCommand(ILI9341_POWER_CONTROL_1);
	TFT_write8(0x23); // 4.60V (default is 4.50V)
	TFT_writeCommand(ILI9341_POWER_CONTROL_2);
	TFT_write8(0x10); // Default
	TFT_writeCommand(ILI9341_VCOM_CONTROL_1);
	TFT_write16(0x2B2B); // 3.775V | -1.425V
	TFT_writeCommand(ILI9341_VCOM_CONTROL_2);
	TFT_write8(0x0C); // ~nVM | VMH – 52 | VML – 52
	TFT_writeCommand(ILI9341_MEMORY_ACCESS_CONTROL);
	TFT_write8(ILI9341_MAC_BGR | TFT_Rotation);
	TFT_writeCommand(ILI9341_PIXEL_FORMAT_SET);
	TFT_write8(ILI9341_PIXEL_FORMAT_RGB_16BPP | ILI9341_PIXEL_FORMAT_MCU_16BPP);
	TFT_writeCommand(ILI9341_FRAME_CONTROL_NORMAL_MODE);
	TFT_write16(0x001B); // fosc | 70 Hz (default)
	TFT_writeCommand(ILI9341_SLEEP_OUT);
	TFT_clearActive();

	_delay_ms(150);

	TFT_setActive();
	TFT_writeCommand(ILI9341_DISPLAY_ON);
	TFT_clearActive();
}

u32 TFT_readId() {
	TFT_setActive();
	TFT_writeCommand(ILI9341_READ_ID4);
	TFT_setReadDirection();
	TFT_read8();
	u32 result = TFT_read24();
	TFT_clearActive();
	TFT_setWriteDirection();
	return result;
}

void TFT_setScroll(u16 offset) {
	TFT_setActive();
	TFT_writeCommand(ILI9341_VERTICAL_SCROLLING_START_ADDRESS);
	TFT_write16(offset);
	TFT_clearActive();
}

void TFT_fillRect(i16 x, i16 y, i16 w, i16 h, u16 color) {
	if ((x >= TFT_Width) || (y >= TFT_Height)) return;
	if (x < 0) { w += x; x = 0; }
	if (y < 0) { h += y; y = 0; }
	if ((x + w - 1) >= TFT_Width)  { w = TFT_Width  - x; }
	if ((y + h - 1) >= TFT_Height) { h = TFT_Height - y; }

	TFT_setActive();
	TFT_setAddr(x, y, x+w-1, y+h-1);
	TFT_writeCommand(ILI9341_MEMORY_WRITE);

	u8 colorHigh = (color >> 8) & 0xFF;
	u8 colorLow  = (color >> 0) & 0xFF;

	if (colorHigh == colorLow) {
		TFT_write8(colorLow);
		TFT_resend();
		for (x = 0; x < w - 1; ++x) {
			TFT_resend();
			TFT_resend();
		}
		for (y = 0; y < h - 1; ++y) {
			for (x = 0; x < w; ++x) {
				TFT_resend();
				TFT_resend();
			}
		}
	} else {
		for (y = 0; y < h; ++y) {
			for (x = 0; x < w; ++x) {
				TFT_write8(colorHigh);
				TFT_write8(colorLow);
			}
		}
	}

	TFT_clearActive();
}

void TFT_drawPixel(i16 x, i16 y, u16 color) {
	if ((x < 0) || (x >= TFT_Width) || (y < 0) || (y >= TFT_Height)) return;

	TFT_setActive();
	TFT_setAddr(x, y, x, y);
	TFT_writeCommand(ILI9341_MEMORY_WRITE);
	TFT_write16(color);
	TFT_clearActive();
}

void TFT_drawFastVLine(i16 x, i16 y, i16 h, u16 color) {
	if ((x >= TFT_Width) || (x < 0) || (y >= TFT_Height)) return;
	if (y < 0) { h += y; y = 0; }
	if ((y + h - 1) >= TFT_Height) { h = TFT_Height - y; }

	TFT_setActive();
	TFT_setAddr(x, y, x, y + h - 1);
	TFT_writeCommand(ILI9341_MEMORY_WRITE);
	while (h-- > 0) {
		TFT_write16(color);
	}
	TFT_clearActive();
}

void TFT_drawFastHLine(i16 x, i16 y, i16 w, u16 color) {
	if ((x >= TFT_Width) || (y >= TFT_Height) || (y < 0)) return;
	if (x < 0) { w += x; x = 0; }
	if ((x + w - 1) >= TFT_Width) { w = TFT_Width - x; }

	TFT_setActive();
	TFT_setAddr(x, y, x + w - 1, y);
	TFT_writeCommand(ILI9341_MEMORY_WRITE);
	while (w-- > 0) {
		TFT_write16(color);
	}
	TFT_clearActive();
}

void TFT_fillScreen(u16 color) {
	TFT_fillRect(0, 0, TFT_Width, TFT_Height, color);
}

void TFT_drawCharacter(i16 x, i16 y, u8 c, u16 fgcolor, u16 bgcolor, u8 size) {
	if ((x >= TFT_Width) ||
		(y >= TFT_Height) ||
		((x + (6 * size) - 1) < 0) ||
		((y + (8 * size) - 1) < 0)) {
		return;
	}

	if (fgcolor == bgcolor) {
		// This transparent approach is only about 20% faster
		if (size == 1) {
			uint8_t mask = 0x01;
			i16 xoff, yoff;
			for (yoff=0; yoff < 8; yoff++) {
				uint8_t line = 0;
				for (xoff=0; xoff < 5; xoff++) {
					if (pgm_read_byte(&TFT_font[c * 5 + xoff]) & mask) line |= 1;
					line <<= 1;
				}
				line >>= 1;
				xoff = 0;
				while (line) {
					if (line == 0x1F) {
						TFT_drawFastHLine(x + xoff, y + yoff, 5, fgcolor);
						break;
					} else if (line == 0x1E) {
						TFT_drawFastHLine(x + xoff, y + yoff, 4, fgcolor);
						break;
					} else if ((line & 0x1C) == 0x1C) {
						TFT_drawFastHLine(x + xoff, y + yoff, 3, fgcolor);
						line <<= 4;
						xoff += 4;
					} else if ((line & 0x18) == 0x18) {
						TFT_drawFastHLine(x + xoff, y + yoff, 2, fgcolor);
						line <<= 3;
						xoff += 3;
					} else if ((line & 0x10) == 0x10) {
						TFT_drawPixel(x + xoff, y + yoff, fgcolor);
						line <<= 2;
						xoff += 2;
					} else {
						line <<= 1;
						xoff += 1;
					}
				}
				mask = mask << 1;
			}
		} else {
			uint8_t mask = 0x01;
			i16 xoff, yoff;
			for (yoff=0; yoff < 8; yoff++) {
				uint8_t line = 0;
				for (xoff=0; xoff < 5; xoff++) {
					if (pgm_read_byte(&TFT_font[c * 5 + xoff]) & mask) line |= 1;
					line <<= 1;
				}
				line >>= 1;
				xoff = 0;
				while (line) {
					if (line == 0x1F) {
						TFT_fillRect(x + xoff * size, y + yoff * size, 5 * size, size, fgcolor);
						break;
					} else if (line == 0x1E) {
						TFT_fillRect(x + xoff * size, y + yoff * size, 4 * size, size, fgcolor);
						break;
					} else if ((line & 0x1C) == 0x1C) {
						TFT_fillRect(x + xoff * size, y + yoff * size, 3 * size, size, fgcolor);
						line <<= 4;
						xoff += 4;
					} else if ((line & 0x18) == 0x18) {
						TFT_fillRect(x + xoff * size, y + yoff * size, 2 * size, size, fgcolor);
						line <<= 3;
						xoff += 3;
					} else if ((line & 0x10) == 0x10) {
						TFT_fillRect(x + xoff * size, y + yoff * size, size, size, fgcolor);
						line <<= 2;
						xoff += 2;
					} else {
						line <<= 1;
						xoff += 1;
					}
				}
				mask = mask << 1;
			}
		}
	} else {
		TFT_setActive();
		TFT_setAddr(x, y, x + 6 * size - 1, y + 8 * size - 1);
		TFT_writeCommand(ILI9341_MEMORY_WRITE);
		uint8_t xr, yr;
		uint8_t mask = 0x01;
		u16 color;
		for (y=0; y < 8; y++) {
			for (yr=0; yr < size; yr++) {
				for (x=0; x < 5; x++) {
					if (pgm_read_byte(&TFT_font[c * 5 + x]) & mask) {
						color = fgcolor;
					} else {
						color = bgcolor;
					}
					for (xr=0; xr < size; xr++) {
						TFT_write16(color);
					}
				}
				for (xr=0; xr < size; xr++) {
					TFT_write16(bgcolor);
				}
			}
			mask = mask << 1;
		}
		TFT_clearActive();
	}
}

