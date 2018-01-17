#pragma once

#include "defs.h"

#define USB_DEVICE_STATE GPIOR0

#if (F_CPU == 8000000)
	#define USB_PLL_PRESCALE 0
#elif (F_CPU == 16000000)
	#define USB_PLL_PRESCALE _BV(PINDIV)
#endif

#define USB_ENDPOINT_CONTROL       0
#define USB_ENDPOINT_CONTROL_SIZE  64

/* ATmega32U4 datasheet, p. 287 */
#define USB_CFG0X_ENDPOINT_DIRECTION_OUT (0 << EPDIR)
#define USB_CFG0X_ENDPOINT_DIRECTION_IN  (1 << EPDIR)

#define USB_CFG0X_ENDPOINT_TYPE_CONTROL     (0 << EPTYPE0)
#define USB_CFG0X_ENDPOINT_TYPE_ISOCHRONOUS (1 << EPTYPE0)
#define USB_CFG0X_ENDPOINT_TYPE_BULK        (2 << EPTYPE0)
#define USB_CFG0X_ENDPOINT_TYPE_INTERRUPT   (3 << EPTYPE0)

#define USB_CFG1X_ENDPOINT_SIZE_8      (0 << EPSIZE0)
#define USB_CFG1X_ENDPOINT_SIZE_16     (1 << EPSIZE0)
#define USB_CFG1X_ENDPOINT_SIZE_32     (2 << EPSIZE0)
#define USB_CFG1X_ENDPOINT_SIZE_64     (3 << EPSIZE0)
#define USB_CFG1X_ENDPOINT_SIZE_128    (4 << EPSIZE0)
#define USB_CFG1X_ENDPOINT_SIZE_256    (5 << EPSIZE0)
#define USB_CFG1X_ENDPOINT_SIZE_512    (6 << EPSIZE0)
#define _USB_CFG1X_ENDPOINT_SIZE(bytes) USB_CFG1X_ENDPOINT_SIZE_##bytes
#define USB_CFG1X_ENDPOINT_SIZE(bytes) _USB_CFG1X_ENDPOINT_SIZE(bytes)

void USB_enablePLL();
void USB_disablePLL();
void USB_setAddress(u8 address);
void USB_enableAddress();
bool USB_configureEndpoint(u8 number, u8 flags0, u8 flags1);
void USB_resetEndpoint(u8 number);
void USB_resetEndpointDataToggle();
bool USB_isEndpointEnabled();
void USB_selectEndpoint(u8 number);
u8 USB_getSelectedEndpoint();
u8 USB_readByteFromEndpoint();
void USB_readFromEndpoint(void *buffer, u8 length);
void USB_writeByteToEndpoint(u8 byte);
u16 USB_bytesInEndpoint();
void USB_stallEndpoint();
void USB_clearEndpointStall();
bool USB_isEndpointStalled();
bool USB_isINReady();
bool USB_waitForINReady();
void USB_sendIN();
bool USB_isOUTReady();
bool USB_waitForOUTReady();
void USB_clearOUT();
bool USB_isSETUPReady();
void USB_clearSETUP();
bool USB_transferData(const void *buffer, u8 length, bool fromPROGMEM, u8 endpointSize);
