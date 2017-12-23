#pragma once

#include "defs.h"

void transmitLeds();
void setLed(u8 index, u32 color);
void setAllLeds(u32 color);
void setLedPattern(u32 *colors);
