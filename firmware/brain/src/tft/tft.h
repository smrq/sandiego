#pragma once

#include "defs.h"
#include "./tft_hardware.h"
#include "./tft_font.h"

void TFT_init();
void TFT_enableBacklight();
void TFT_disableBacklight();
u32 TFT_readId();
void TFT_setScroll(u16 offset);
void TFT_fillRect(i16 x, i16 y, i16 w, i16 h, u16 color);
void TFT_drawPixel(i16 x, i16 y, u16 color);
void TFT_drawFastVLine(i16 x, i16 y, i16 h, u16 color);
void TFT_drawFastHLine(i16 x, i16 y, i16 w, u16 color);
void TFT_fillScreen(u16 color);
void TFT_drawCharacter(i16 x, i16 y, u8 c, u16 fgcolor, u16 bgcolor, u8 size);
