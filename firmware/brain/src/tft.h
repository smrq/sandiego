#pragma once

#include "defs.h"

#define TFT_WIDTH  240
#define TFT_HEIGHT 320

/* ILI9341 datasheet v1.02, section 8.1. Command List, p. 83 */
#define ILI9341_NOP                                       0x00
#define ILI9341_SOFTWARE_RESET                            0x01
#define ILI9341_READ_DISPLAY_IDENTIFICATION_INFORMATION   0x04
#define ILI9341_READ_DISPLAY_STATUS                       0x09
#define ILI9341_READ_DISPLAY_POWER_MODE                   0x0A
#define ILI9341_READ_DISPLAY_MADCTL                       0x0B
#define ILI9341_READ_DISPLAY_PIXEL_FORMAT                 0x0C
#define ILI9341_READ_DISPLAY_IMAGE_FORMAT                 0x0D
#define ILI9341_READ_DISPLAY_SIGNAL_MODE                  0x0E
#define ILI9341_READ_DISPLAY_SELF_DIAGNOSTIC_RESULT       0x0F
#define ILI9341_ENTER_SLEEP_MODE                          0x10
#define ILI9341_SLEEP_OUT                                 0x11
#define ILI9341_PARTIAL_MODE_ON                           0x12
#define ILI9341_NORMAL_DISPLAY_MODE_ON                    0x13
#define ILI9341_DISPLAY_INVERSION_OFF                     0x20
#define ILI9341_DISPLAY_INVERSION_ON                      0x21
#define ILI9341_GAMMA_SET                                 0x26
#define ILI9341_DISPLAY_OFF                               0x28
#define ILI9341_DISPLAY_ON                                0x29
#define ILI9341_COLUMN_ADDRESS_SET                        0x2A
#define ILI9341_PAGE_ADDRESS_SET                          0x2B
#define ILI9341_MEMORY_WRITE                              0x2C
#define ILI9341_COLOR_SET                                 0x2D
#define ILI9341_MEMORY_READ                               0x2E
#define ILI9341_PARTIAL_AREA                              0x30
#define ILI9341_VERTICAL_SCROLLING_DEFINITION             0x33
#define ILI9341_TEARING_EFFECT_LINE_OFF                   0x34
#define ILI9341_TEARING_EFFECT_LINE_ON                    0x35
#define ILI9341_MEMORY_ACCESS_CONTROL                     0x36
#define ILI9341_VERTICAL_SCROLLING_START_ADDRESS          0x37
#define ILI9341_IDLE_MODE_OFF                             0x38
#define ILI9341_IDLE_MODE_ON                              0x39
#define ILI9341_PIXEL_FORMAT_SET                          0x3A
#define ILI9341_WRITE_MEMORY_CONTINUE                     0x3C
#define ILI9341_READ_MEMORY_CONTINUE                      0x3E
#define ILI9341_SET_TEAR_SCANLINE                         0x44
#define ILI9341_GET_SCANLINE                              0x45
#define ILI9341_WRITE_DISPLAY_BRIGHTNESS                  0x51
#define ILI9341_READ_DISPLAY_BRIGHTNESS                   0x52
#define ILI9341_WRITE_CTRL_DISPLAY                        0x53
#define ILI9341_READ_CTRL_DISPLAY                         0x54
#define ILI9341_WRITE_CONTENT_ADAPTIVE_BRIGHTNESS_CONTROL 0x55
#define ILI9341_READ_CONTENT_ADAPTIVE_BRIGHTNESS_CONTROL  0x56
#define ILI9341_WRITE_CABC_MINIMUM_BRIGHTNESS             0x5E
#define ILI9341_READ_CABC_MINIMUM_BRIGHTNESS              0x5F
#define ILI9341_READ_ID1                                  0xDA
#define ILI9341_READ_ID2                                  0xDB
#define ILI9341_READ_ID3                                  0xDC

#define ILI9341_RGB_INTERFACE_SIGNAL_CONTROL              0xB0
#define ILI9341_FRAME_CONTROL_NORMAL_MODE                 0xB1
#define ILI9341_FRAME_CONTROL_IDLE_MODE                   0xB2
#define ILI9341_FRAME_CONTROL_PARTIAL_MODE                0xB3
#define ILI9341_DISPLAY_INVERSION_CONTROL                 0xB4
#define ILI9341_BLANKING_PORCH_CONTROL                    0xB5
#define ILI9341_DISPLAY_FUNCTION_CONTROL                  0xB6
#define ILI9341_ENTRY_MODE_SET                            0xB7
#define ILI9341_BACKLIGHT_CONTROL_1                       0xB8
#define ILI9341_BACKLIGHT_CONTROL_2                       0xB9
#define ILI9341_BACKLIGHT_CONTROL_3                       0xBA
#define ILI9341_BACKLIGHT_CONTROL_4                       0xBB
#define ILI9341_BACKLIGHT_CONTROL_5                       0xBC
//      ILI9341_BACKLIGHT_CONTROL_6                       0xBD
#define ILI9341_BACKLIGHT_CONTROL_7                       0xBE
#define ILI9341_BACKLIGHT_CONTROL_8                       0xBF
#define ILI9341_POWER_CONTROL_1                           0xC0
#define ILI9341_POWER_CONTROL_2                           0xC1
#define ILI9341_VCOM_CONTROL_1                            0xC5
#define ILI9341_VCOM_CONTROL_2                            0xC7
#define ILI9341_NV_MEMORY_WRITE                           0xD0
#define ILI9341_NV_MEMORY_PROTECTION_KEY                  0xD1
#define ILI9341_NV_MEMORY_STATUS_READ                     0xD2
#define ILI9341_READ_ID4                                  0xD3
#define ILI9341_POSITIVE_GAMMA_CORRECTION                 0xE0
#define ILI9341_NEGATIVE_GAMMA_CORRECTION                 0xE1
#define ILI9341_DIGITAL_GAMMA_CONTROL_1                   0xE2
#define ILI9341_DIGITAL_GAMMA_CONTROL_2                   0xE3
#define ILI9341_INTERFACE_CONTROL                         0xF6

#define ILI9341_MAC_MY  _BV(7)
#define ILI9341_MAC_MX  _BV(6)
#define ILI9341_MAC_MV  _BV(5)
#define ILI9341_MAC_ML  _BV(4)
#define ILI9341_MAC_BGR _BV(3)
#define ILI9341_MAC_MH  _BV(2)

#define ILI9341_MAC_ROTATION_0   ILI9341_MAC_MX
#define ILI9341_MAC_ROTATION_90  ILI9341_MAC_MV
#define ILI9341_MAC_ROTATION_180 ILI9341_MAC_MY
#define ILI9341_MAC_ROTATION_270 (ILI9341_MAC_MX | ILI9341_MAC_MY | ILI9341_MAC_MV)

#define ILI9341_PIXEL_FORMAT_RGB_18BPP 0x60
#define ILI9341_PIXEL_FORMAT_RGB_16BPP 0x50
#define ILI9341_PIXEL_FORMAT_MCU_18BPP 0x06
#define ILI9341_PIXEL_FORMAT_MCU_16BPP 0x05

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
