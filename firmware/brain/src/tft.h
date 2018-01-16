#pragma once

#include "defs.h"

#define ILI9341_SOFTRESET          0x01
#define ILI9341_SLEEPIN            0x10
#define ILI9341_SLEEPOUT           0x11
#define ILI9341_NORMALDISP         0x13
#define ILI9341_INVERTOFF          0x20
#define ILI9341_INVERTON           0x21
#define ILI9341_GAMMASET           0x26
#define ILI9341_DISPLAYOFF         0x28
#define ILI9341_DISPLAYON          0x29
#define ILI9341_COLADDRSET         0x2A
#define ILI9341_PAGEADDRSET        0x2B
#define ILI9341_MEMORYWRITE        0x2C
#define ILI9341_PIXELFORMAT        0x3A
#define ILI9341_FRAMECONTROL       0xB1
#define ILI9341_DISPLAYFUNC        0xB6
#define ILI9341_ENTRYMODE          0xB7
#define ILI9341_POWERCONTROL1      0xC0
#define ILI9341_POWERCONTROL2      0xC1
#define ILI9341_VCOMCONTROL1       0xC5
#define ILI9341_VCOMCONTROL2       0xC7
#define ILI9341_MEMCONTROL         0x36
#define ILI9341_MADCTL             0x36

#define ILI9341_MADCTL_MY          0x80
#define ILI9341_MADCTL_MX          0x40
#define ILI9341_MADCTL_MV          0x20
#define ILI9341_MADCTL_ML          0x10
#define ILI9341_MADCTL_RGB         0x00
#define ILI9341_MADCTL_BGR         0x08
#define ILI9341_MADCTL_MH          0x04

void TFT_init();
void TFT_reset();
u32 TFT_readId();
void TFT_begin();
void TFT_fillScreen(uint16_t color);
void TFT_enableBacklight();
void TFT_disableBacklight();
