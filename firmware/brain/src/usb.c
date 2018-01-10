#include "defs.h"
#include "debug.h"
#include "keys.h"
#include "usb.h"

u8 USB_ConfigurationNumber = 0;
bool USB_RemoteWakeupEnabled = false;
bool USB_UsingReportProtocol = true;

/*
	The recommended default idle rate (rate when the device is initialized) is 500
	milliseconds for keyboards (delay before first repeat rate) and infinity for
	joysticks and mice.
		- USB HID 1.11 Specification, p. 53
*/
u16 USB_IdleTimeoutDuration = 500;
u16 USB_IdleTimeoutRemaining = 0;

local void HID_sendReport() {
	static union {
		USB_bootKeyboardReport_t boot;
		USB_nkroKeyboardReport_t nkro;
	} previousReport = { 0 };

	union {
		USB_bootKeyboardReport_t boot;
		USB_nkroKeyboardReport_t nkro;
	} report = { 0 };
	u8 reportSize;

	if (USB_UsingReportProtocol) {
		populateNkroKeyboardReport(&report.nkro);
		reportSize = sizeof(report.nkro);
	} else {
		populateBootKeyboardReport(&report.boot);
		reportSize = sizeof(report.boot);
	}

	bool shouldSend;
	if (USB_IdleTimeoutDuration && !USB_IdleTimeoutRemaining) {
		USB_IdleTimeoutRemaining = USB_IdleTimeoutDuration;
		shouldSend = true;
	} else {
		shouldSend = memcmp(&previousReport, &report, sizeof(report)) != 0;
	}

	if (!shouldSend) {
		return;
	}

	USB_selectEndpoint(USB_ENDPOINT_KEYBOARD_IN);
	if (!USB_isINReady()) {
		return;
	}
	if (!USB_transferData(&report, reportSize, false, USB_ENDPOINT_KEYBOARD_SIZE)) return;

	memcpy(&previousReport, &report, sizeof(report));
}

local void HID_receiveReport(void) {
	USB_selectEndpoint(USB_ENDPOINT_KEYBOARD_OUT);
	if (!USB_isOUTReady()) {
		return;
	}
	USB_ledReport_t report = USB_readByteFromEndpoint();
	USB_sendOUT();

	processLEDReport(report);
}

void USB_init() {
	/*
		Power On the USB interface
			- Power-On USB pads regulator
			- Configure PLL interface
			- Enable PLL
			- Check PLL lock
			- Enable USB interface
			- Configure USB interface (USB speed, Endpoints configuration...)
			- Wait for USB VBUS information connection
			- Attach USB device

			- ATmega32U4 datasheet, section 21.12 USB Software Operating Modes, p. 266
	*/

	UHWCON = _BV(UVREGE);
	USBCON = _BV(USBE) | _BV(FRZCLK);
	PLLFRQ = _BV(PLLUSB) | _BV(PDIV3) | _BV(PDIV1);
	USB_enablePLL();
	USBCON = (USBCON & ~_BV(FRZCLK)) | _BV(OTGPADE) | _BV(VBUSTE);
	UDCON = 0; // Attach, enable full speed mode
	UDIEN = _BV(SUSPE) | _BV(EORSTE);

	USB_configureEndpoint(USB_ENDPOINT_CONTROL,
		USB_CFG0X_ENDPOINT_TYPE_CONTROL | USB_CFG0X_ENDPOINT_DIRECTION_OUT,
		USB_CFG1X_ENDPOINT_SIZE(USB_ENDPOINT_CONTROL_SIZE));
}

void USB_update() {
	if (USB_DEVICE_STATE != USB_DeviceState_Configured) {
		return;
	}
	HID_sendReport();
	HID_receiveReport();
}
