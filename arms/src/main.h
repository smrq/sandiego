#pragma once

#include "defs.h"

/*
	Table 14-5. Relationship Between SCK and the Oscillator Frequency
	SPI2X SPR1 SPR0 SCK Frequency
	0     0    0    fosc/4
	0     0    1    fosc/16
	0     1    0    fosc/64
	0     1    1    fosc/128
	1     0    0    fosc/2
	1     0    1    fosc/8
	1     1    0    fosc/32
	1     1    1    fosc/64
	- ATtiny datasheet, p. 127
*/
#define SPCR_CLOCK_SETTINGS ((0 << SPR0) | (0 << SPR1))
#define SPSR_CLOCK_SETTINGS (1 << SPI2X)
