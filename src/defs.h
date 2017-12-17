#pragma once

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>
#include <stdbool.h>

#define local static

#define PIN_SS 2
#define PIN_MOSI 3
#define PIN_SCK 5

#define enableGlobalInterrupts sei
#define disableGlobalInterrupts cli
