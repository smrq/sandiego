#include "defs.h"
#include "./usb.h"

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
	union {
		USB_bootKeyboardReport_t boot;
		USB_nkroKeyboardReport_t nkro;
	} report = { 0 };
	u8 reportSize;
	bool shouldSend;

	if (USB_UsingReportProtocol) {
		shouldSend = populateNkroKeyboardReport(&report.nkro);
		reportSize = sizeof(report.nkro);
	} else {
		shouldSend = populateBootKeyboardReport(&report.boot);
		reportSize = sizeof(report.boot);
	}

	if (USB_IdleTimeoutDuration && !USB_IdleTimeoutRemaining) {
		USB_IdleTimeoutRemaining = USB_IdleTimeoutDuration;
		shouldSend = true;
	}

	if (!shouldSend) {
		return;
	}

	USB_selectEndpoint(USB_ENDPOINT_KEYBOARD_IN);
	USB_transferData(&report, reportSize, false, USB_ENDPOINT_KEYBOARD_SIZE);
}

local void HID_receiveReport(void) {
	USB_selectEndpoint(USB_ENDPOINT_KEYBOARD_OUT);
	if (!USB_isOUTReady()) {
		return;
	}
	USB_ledReport_t report = USB_readByteFromEndpoint();
	USB_clearOUT();

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

//-----------------------------

local void setNkroKey(USB_nkroKeyboardReport_t *report, u8 key) {
	if (key) {
		report->keys[key / 8] |= _BV(key % 8);
	}
}

local void setNkroChar(USB_nkroKeyboardReport_t *report, u8 c) {
	if (c >= 'a' && c <= 'z') {
		setNkroKey(report, HID_KEYBOARD_A + (c - 'a'));
	}
	if (c >= 'A' && c <= 'Z') {
		setNkroKey(report, HID_KEYBOARD_A + (c - 'a'));
		setNkroKey(report, HID_KEYBOARD_LSHIFT);
	}
	if (c == '0') {
		setNkroKey(report, HID_KEYBOARD_0_RPAREN);
	}
	if (c >= '1' && c <= '9') {
		setNkroKey(report, HID_KEYBOARD_1_BANG + (c - '1'));
	}
	if (c == ' ') {
		setNkroKey(report, HID_KEYBOARD_SPACE);
	}
	if (c == '\n') {
		setNkroKey(report, HID_KEYBOARD_ENTER);
	}
}

void USB_debugSendString(char *str, u8 length) {
	USB_selectEndpoint(USB_ENDPOINT_KEYBOARD_IN);

	USB_nkroKeyboardReport_t lastReport = { 0 };
	USB_transferData(&lastReport, sizeof(lastReport), false, USB_ENDPOINT_KEYBOARD_SIZE);

	while (length) {
		USB_nkroKeyboardReport_t report = { 0 };
		setNkroChar(&report, str[0]);

		if (memcmp(&report, &lastReport, sizeof(report)) == 0) {
			memset(&lastReport, 0, sizeof(lastReport));
			USB_transferData(&lastReport, sizeof(lastReport), false, USB_ENDPOINT_KEYBOARD_SIZE);
		}

		USB_transferData(&report, sizeof(report), false, USB_ENDPOINT_KEYBOARD_SIZE);

		memcpy(&lastReport, &report, sizeof(lastReport));

		++str;
		--length;
	}

	memset(&lastReport, 0, sizeof(lastReport));
	USB_transferData(&lastReport, sizeof(lastReport), false, USB_ENDPOINT_KEYBOARD_SIZE);
}
