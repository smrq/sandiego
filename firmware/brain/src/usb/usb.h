#pragma once

#include "defs.h"
#include "./usb-descriptors.h"
#include "./usb-hardware.h"
#include "./usb-reports.h"
#include "./usb-spec.h"

/*
	A device can have more than one configuration, though only one at a time,
	and to change configuration the whole device would have to stop functioning.
	Different configurations might be used, for example, to specify different
	current requirements, as the current required is defined in the configuration
	descriptor.

	However it is not common to have more than one configuration. Windows standard
	drivers will always select the first configuration so there is not a lot of point.

	- http://www.usbmadesimple.co.uk/ums_4.htm
*/
#define USB_CONFIGURATION_COUNT    1
#define USB_CONFIGURATION_KEYBOARD 1

#define USB_INTERFACE_COUNT        1
#define USB_INTERFACE_KEYBOARD     1

#define USB_ENDPOINT_COUNT         2
#define USB_ENDPOINT_KEYBOARD_IN   1
#define USB_ENDPOINT_KEYBOARD_OUT  2
#define USB_ENDPOINT_KEYBOARD_SIZE 32

#define USB_VENDOR_ID              0xF055
#define USB_PRODUCT_ID             0x2042
#define USB_RELEASE_NUMBER         BCD(2,0,0)

extern u8 USB_ConfigurationNumber;
extern bool USB_RemoteWakeupEnabled;
extern bool USB_UsingReportProtocol;
extern u16 USB_IdleTimeoutDuration;
extern u16 USB_IdleTimeoutRemaining;

void USB_init();
void USB_update();

void USB_debugSendString(char *str, u8 length);
