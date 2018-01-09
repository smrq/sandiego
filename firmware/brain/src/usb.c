#include <LUFA/Drivers/USB/USB.h>
#include "defs.h"
#include "debug.h"
#include "keys.h"
#include "usb-descriptors.h"
#include "usb.h"
#include "usb-reports.h"

local bool usingReportProtocol = true;
local u16 idleTimeoutDuration = 500;
local u16 idleTimeoutRemaining = 0;

void EVENT_USB_Device_Connect() {
	/*
		When initialized, all devices default to report protocol.
			- Device Class Definition for HID 1.11, section 7.2.6, p. 54
	*/
	usingReportProtocol = true;
	debug_output(DEBUG_USB_ENUMERATING);
}

void EVENT_USB_Device_Disconnect() {
	debug_output(DEBUG_USB_NOTREADY);
}

/** Event handler for the USB_ConfigurationChanged event. This is fired when the host sets the current configuration
 *  of the USB device after enumeration, and configures the keyboard device endpoints.
 */
void EVENT_USB_Device_ConfigurationChanged(void)
{
	bool success = true;
	success &= Endpoint_ConfigureEndpoint(KEYBOARD_IN_EPADDR, EP_TYPE_INTERRUPT, KEYBOARD_EPSIZE, 1);
	success &= Endpoint_ConfigureEndpoint(KEYBOARD_OUT_EPADDR, EP_TYPE_INTERRUPT, KEYBOARD_EPSIZE, 1);

	USB_Device_EnableSOFEvents();

	debug_output(success ? DEBUG_USB_READY : DEBUG_USB_ERROR);
}

void EVENT_USB_Device_ControlRequest(void) {
	switch (USB_ControlRequest.bRequest) {
		/* See: Device Class Definition for HID 1.11, section 7.2.1, p. 51 */
		case HID_REQ_GetReport:
			if (USB_ControlRequest.bmRequestType == (REQDIR_DEVICETOHOST | REQTYPE_CLASS | REQREC_INTERFACE)) {
				union {
					USB_bootKeyboardReport_t boot;
					USB_nkroKeyboardReport_t nkro;
				} report = { 0 };

				if (usingReportProtocol) {
					populateNkroKeyboardReport(&report.nkro);
				} else {
					populateBootKeyboardReport(&report.boot);
				}

				Endpoint_ClearSETUP();
				Endpoint_Write_Control_Stream_LE(&report, usingReportProtocol ? sizeof(report.nkro) : sizeof(report.boot));
				Endpoint_ClearOUT();
			}
			break;

		/* See: Device Class Definition for HID 1.11, section 7.2.2, p. 52 */
		case HID_REQ_SetReport:
			if (USB_ControlRequest.bmRequestType == (REQDIR_HOSTTODEVICE | REQTYPE_CLASS | REQREC_INTERFACE)) {
				Endpoint_ClearSETUP();
				while (!Endpoint_IsOUTReceived()) {
					if (USB_DeviceState == DEVICE_STATE_Unattached) {
						return;
					}
				}
				USB_ledReport_t report = Endpoint_Read_8();
				Endpoint_ClearOUT();
				Endpoint_ClearStatusStage();

				processLEDReport(report);
			}

			break;

		/* See: Device Class Definition for HID 1.11, section 7.2.5, p. 54 */
		case HID_REQ_GetProtocol:
			if (USB_ControlRequest.bmRequestType == (REQDIR_DEVICETOHOST | REQTYPE_CLASS | REQREC_INTERFACE)) {
				Endpoint_ClearSETUP();
				Endpoint_Write_8(usingReportProtocol);
				Endpoint_ClearIN();
				Endpoint_ClearStatusStage();
			}

			break;

		/* See: Device Class Definition for HID 1.11, section 7.2.6, p. 54 */
		case HID_REQ_SetProtocol:
			if (USB_ControlRequest.bmRequestType == (REQDIR_HOSTTODEVICE | REQTYPE_CLASS | REQREC_INTERFACE)) {
				Endpoint_ClearSETUP();
				Endpoint_ClearStatusStage();

				/* Set or clear the flag depending on what the host indicates that the current Protocol should be */
				usingReportProtocol = (USB_ControlRequest.wValue != 0);
			}

			break;

		/* See: Device Class Definition for HID 1.11, section 7.2.4, p. 52-53 */
		case HID_REQ_SetIdle:
			if (USB_ControlRequest.bmRequestType == (REQDIR_HOSTTODEVICE | REQTYPE_CLASS | REQREC_INTERFACE)) {
				Endpoint_ClearSETUP();
				Endpoint_ClearStatusStage();
				idleTimeoutDuration = ((USB_ControlRequest.wValue & 0xFF00) >> 6);
			}

			break;

		/* See: Device Class Definition for HID 1.11, section 7.2.3, p. 52 */
		case HID_REQ_GetIdle:
			if (USB_ControlRequest.bmRequestType == (REQDIR_DEVICETOHOST | REQTYPE_CLASS | REQREC_INTERFACE)) {
				Endpoint_ClearSETUP();
				Endpoint_Write_8(idleTimeoutDuration >> 2);
				Endpoint_ClearIN();
				Endpoint_ClearStatusStage();
			}
			break;
	}
}

void EVENT_USB_Device_StartOfFrame()
{
	if (idleTimeoutRemaining) {
		idleTimeoutRemaining--;
	}
}

local void HID_sendReport() {
	static union {
		USB_bootKeyboardReport_t boot;
		USB_nkroKeyboardReport_t nkro;
	} previousReport = { 0 };

	union {
		USB_bootKeyboardReport_t boot;
		USB_nkroKeyboardReport_t nkro;
	} report = { 0 };

	if (usingReportProtocol) {
		populateNkroKeyboardReport(&report.nkro);
	} else {
		populateBootKeyboardReport(&report.boot);
	}

	bool shouldSend;
	if (idleTimeoutDuration && !idleTimeoutRemaining) {
		idleTimeoutRemaining = idleTimeoutDuration;
		shouldSend = true;
	} else {
		shouldSend = memcmp(&previousReport, &report, sizeof(report)) != 0;
	}

	Endpoint_SelectEndpoint(KEYBOARD_IN_EPADDR);
	if (Endpoint_IsReadWriteAllowed() && shouldSend) {
		memcpy(&previousReport, &report, sizeof(report));

		Endpoint_Write_Stream_LE(
			&report,
			usingReportProtocol ? sizeof(report.nkro) : sizeof(report.boot),
			NULL);

		Endpoint_ClearIN();
	}
}

local void HID_receiveReport(void)
{
	Endpoint_SelectEndpoint(KEYBOARD_OUT_EPADDR);
	if (!Endpoint_IsOUTReceived()) {
		return;
	}

	if (Endpoint_IsReadWriteAllowed()) {
		USB_ledReport_t report = Endpoint_Read_8();
		processLEDReport(report);
	}

	Endpoint_ClearOUT();
}

void USB_init() {
	USB_Init();
}

void USB_update() {
	if (USB_DeviceState != DEVICE_STATE_Configured) {
		return;
	}
	HID_sendReport();
	HID_receiveReport();
}
