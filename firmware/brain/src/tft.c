#include "defs.h"
#include "tft.h"

#define TFTWIDTH   240
#define TFTHEIGHT  320

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
	TFT_writeCommand(ILI9341_COLADDRSET);
	TFT_write16(x0);
	TFT_write16(x1);
	TFT_writeCommand(ILI9341_PAGEADDRSET);
	TFT_write16(y0);
	TFT_write16(y1);
}

static const uint8_t init_commands[] = {
	4, 0xEF, 0x03, 0x80, 0x02,
	4, 0xCF, 0x00, 0XC1, 0X30,
	5, 0xED, 0x64, 0x03, 0X12, 0X81,
	4, 0xE8, 0x85, 0x00, 0x78,
	6, 0xCB, 0x39, 0x2C, 0x00, 0x34, 0x02,
	2, 0xF7, 0x20,
	3, 0xEA, 0x00, 0x00,
	2, ILI9341_POWERCONTROL1, 0x23, // Power control
	2, ILI9341_POWERCONTROL2, 0x10, // Power control
	3, ILI9341_VCOMCONTROL1, 0x3e, 0x28, // VCM control
	2, ILI9341_VCOMCONTROL2, 0x86, // VCM control2
	2, ILI9341_MEMCONTROL, 0x48, // Memory Access Control
	2, ILI9341_PIXELFORMAT, 0x55,
	3, ILI9341_FRAMECONTROL, 0x00, 0x18,
	4, 0xB6, 0x08, 0x82, 0x27, // Display Function Control
	2, 0xF2, 0x00, // Gamma Function Disable
	2, 0x26, 0x01, // Gamma curve selected
	16, 0xE0, 0x0F, 0x31, 0x2B, 0x0C, 0x0E, 0x08,
		0x4E, 0xF1, 0x37, 0x07, 0x10, 0x03, 0x0E, 0x09, 0x00, // Set Gamma
	16, 0xE1, 0x00, 0x0E, 0x14, 0x03, 0x11, 0x07,
		0x31, 0xC1, 0x48, 0x08, 0x0F, 0x0C, 0x31, 0x36, 0x0F, // Set Gamma
	3, 0xb1, 0x00, 0x10, // FrameRate Control 119Hz
	1, ILI9341_SLEEPOUT,
	0
};

void TFT_init() {
    /*
    	It is necessary to wait 5msec after releasing RESX before sending commands.
    		- ILI9341 datasheet v1.02, section 15.4. Reset Timing, p. 218
    */
    _delay_ms(5);

    const u8 *cmd = init_commands;
	TFT_setActive();
	while (1) {
		if (!(*cmd)) {
			break;
		}
		u8 len = *cmd++;
		TFT_writeCommand(*cmd++);
		for (u8 i = 0; i < (len - 1); ++i) {
			TFT_write8(*cmd++);
		}
	}
	TFT_clearActive();

	_delay_ms(150);

	TFT_setActive();
	TFT_writeCommand(ILI9341_DISPLAYON);
	TFT_clearActive();

	TFT_enableBacklight();
}

u32 TFT_readId() {
	TFT_setActive();
	TFT_writeCommand(0xD3);
	TFT_setReadDirection();
	TFT_read8();
	u32 result = TFT_read24();
	TFT_clearActive();
	TFT_setWriteDirection();
	return result;
}


void TFT_fillRect(i16 x, i16 y, i16 w, i16 h, u16 color) {
	if((x >= TFTWIDTH) || (y >= TFTHEIGHT)) return;
	if(x < 0) {	w += x; x = 0; }
	if(y < 0) {	h += y; y = 0; }
	if((x + w - 1) >= TFTWIDTH)  w = TFTWIDTH  - x;
	if((y + h - 1) >= TFTHEIGHT) h = TFTHEIGHT - y;

	TFT_setActive();
	TFT_setAddr(x, y, x+w-1, y+h-1);
	TFT_writeCommand(ILI9341_MEMORYWRITE);
	for(y=h; y>0; y--) {
		for(x=w; x>0; x--) {
			TFT_write8((color >> 8) & 0xFF);
			TFT_write8((color >> 0) & 0xFF);
		}
	}
	TFT_clearActive();
}

void TFT_fillScreen(uint16_t color) {
	TFT_fillRect(0, 0, TFTWIDTH, TFTHEIGHT, color);
}
