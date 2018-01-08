#include "defs.h"
#include "debug.h"
#include "keys.h"
#include "usb-descriptors.h"
#include "usb-hid.h"
#include <LUFA/Drivers/USB/USB.h>

/** Indicates what report mode the host has requested, true for normal HID reporting mode, \c false for special boot
 *  protocol reporting mode.
 */
static bool UsingReportProtocol = true;

/** Current Idle period. This is set by the host via a Set Idle HID class request to silence the device's reports
 *  for either the entire idle duration, or until the report status changes (e.g. the user presses a key).
 */
static uint16_t IdleCount = 500;

/** Current Idle period remaining. When the IdleCount value is set, this tracks the remaining number of idle
 *  milliseconds. This is separate to the IdleCount timer and is incremented and compared as the host may request
 *  the current idle period via a Get Idle HID class request, thus its value must be preserved.
 */
static uint16_t IdleMSRemaining = 0;

/** Fills the given HID report data structure with the next HID report to send to the host.
 *
 *  \param[out] ReportData  Pointer to a HID report data structure to be filled
 */
local void CreateKeyboardReport(USB_KeyboardReport_Data_t* const report) {
	// TODO: set up real keyboard matrix
	memset(report, 0, sizeof(USB_KeyboardReport_Data_t));

	if (leftKeys.front[0]) {
		report->Modifier = HID_KEYBOARD_MODIFIER_LEFTSHIFT;
	}

	u8 keyCodeIndex = 0;
	if (leftKeys.front[1]) {
		report->KeyCode[keyCodeIndex++] = HID_KEYBOARD_SC_A;
	}
	if (leftKeys.front[2]) {
		report->KeyCode[keyCodeIndex++] = HID_KEYBOARD_SC_B;
	}
	if (leftKeys.front[3]) {
		report->KeyCode[keyCodeIndex++] = HID_KEYBOARD_SC_C;
	}
	if (leftKeys.front[4]) {
		report->KeyCode[keyCodeIndex++] = HID_KEYBOARD_SC_D;
	}
}

/** Processes a received LED report, and updates the board LEDs states to match.
 *
 *  \param[in] LEDReport  LED status report from the host
 */
local void ProcessLEDReport(const uint8_t LEDReport)
{
	u8 value = _BV(1);

	if (LEDReport & HID_KEYBOARD_LED_NUMLOCK) {
		value |= _BV(0);
	}

	if (LEDReport & HID_KEYBOARD_LED_CAPSLOCK) {
		value |= _BV(2);
	}

	if (LEDReport & HID_KEYBOARD_LED_SCROLLLOCK) {
		value |= _BV(3);
	}

	debug_output(value);
}

void EVENT_USB_Device_Connect(void)
{
	debug_output(DEBUG_USB_ENUMERATING);
	UsingReportProtocol = true;
}

void EVENT_USB_Device_Disconnect(void)
{
	debug_output(DEBUG_USB_NOTREADY);
}

/** Event handler for the USB_ConfigurationChanged event. This is fired when the host sets the current configuration
 *  of the USB device after enumeration, and configures the keyboard device endpoints.
 */
void EVENT_USB_Device_ConfigurationChanged(void)
{
	bool ConfigSuccess = true;

	/* Setup HID Report Endpoints */
	ConfigSuccess &= Endpoint_ConfigureEndpoint(KEYBOARD_IN_EPADDR, EP_TYPE_INTERRUPT, KEYBOARD_EPSIZE, 1);
	ConfigSuccess &= Endpoint_ConfigureEndpoint(KEYBOARD_OUT_EPADDR, EP_TYPE_INTERRUPT, KEYBOARD_EPSIZE, 1);

	/* Turn on Start-of-Frame events for tracking HID report period expiry */
	USB_Device_EnableSOFEvents();

	/* Indicate endpoint configuration success or failure */
	debug_output(ConfigSuccess ? DEBUG_USB_READY : DEBUG_USB_ERROR);
}

/** Event handler for the USB_ControlRequest event. This is used to catch and process control requests sent to
 *  the device from the USB host before passing along unhandled control requests to the library for processing
 *  internally.
 */
void EVENT_USB_Device_ControlRequest(void)
{
	/* Handle HID Class specific requests */
	switch (USB_ControlRequest.bRequest)
	{
		case HID_REQ_GetReport:
			if (USB_ControlRequest.bmRequestType == (REQDIR_DEVICETOHOST | REQTYPE_CLASS | REQREC_INTERFACE))
			{
				USB_KeyboardReport_Data_t KeyboardReportData;

				/* Create the next keyboard report for transmission to the host */
				CreateKeyboardReport(&KeyboardReportData);

				Endpoint_ClearSETUP();

				/* Write the report data to the control endpoint */
				Endpoint_Write_Control_Stream_LE(&KeyboardReportData, sizeof(KeyboardReportData));
				Endpoint_ClearOUT();
			}

			break;
		case HID_REQ_SetReport:
			if (USB_ControlRequest.bmRequestType == (REQDIR_HOSTTODEVICE | REQTYPE_CLASS | REQREC_INTERFACE))
			{
				Endpoint_ClearSETUP();

				/* Wait until the LED report has been sent by the host */
				while (!(Endpoint_IsOUTReceived()))
				{
					if (USB_DeviceState == DEVICE_STATE_Unattached)
					  return;
				}

				/* Read in the LED report from the host */
				uint8_t LEDStatus = Endpoint_Read_8();

				Endpoint_ClearOUT();
				Endpoint_ClearStatusStage();

				/* Process the incoming LED report */
				ProcessLEDReport(LEDStatus);
			}

			break;
		case HID_REQ_GetProtocol:
			if (USB_ControlRequest.bmRequestType == (REQDIR_DEVICETOHOST | REQTYPE_CLASS | REQREC_INTERFACE))
			{
				Endpoint_ClearSETUP();

				/* Write the current protocol flag to the host */
				Endpoint_Write_8(UsingReportProtocol);

				Endpoint_ClearIN();
				Endpoint_ClearStatusStage();
			}

			break;
		case HID_REQ_SetProtocol:
			if (USB_ControlRequest.bmRequestType == (REQDIR_HOSTTODEVICE | REQTYPE_CLASS | REQREC_INTERFACE))
			{
				Endpoint_ClearSETUP();
				Endpoint_ClearStatusStage();

				/* Set or clear the flag depending on what the host indicates that the current Protocol should be */
				UsingReportProtocol = (USB_ControlRequest.wValue != 0);
			}

			break;
		case HID_REQ_SetIdle:
			if (USB_ControlRequest.bmRequestType == (REQDIR_HOSTTODEVICE | REQTYPE_CLASS | REQREC_INTERFACE))
			{
				Endpoint_ClearSETUP();
				Endpoint_ClearStatusStage();

				/* Get idle period in MSB, IdleCount must be multiplied by 4 to get number of milliseconds */
				IdleCount = ((USB_ControlRequest.wValue & 0xFF00) >> 6);
			}

			break;
		case HID_REQ_GetIdle:
			if (USB_ControlRequest.bmRequestType == (REQDIR_DEVICETOHOST | REQTYPE_CLASS | REQREC_INTERFACE))
			{
				Endpoint_ClearSETUP();

				/* Write the current idle duration to the host, must be divided by 4 before sent to host */
				Endpoint_Write_8(IdleCount >> 2);

				Endpoint_ClearIN();
				Endpoint_ClearStatusStage();
			}

			break;
	}
}

/** Event handler for the USB device Start Of Frame event. */
void EVENT_USB_Device_StartOfFrame(void)
{
	/* One millisecond has elapsed, decrement the idle time remaining counter if it has not already elapsed */
	if (IdleMSRemaining)
	  IdleMSRemaining--;
}

/** Sends the next HID report to the host, via the keyboard data endpoint. */
local void HID_sendReport(void)
{
	static USB_KeyboardReport_Data_t PrevKeyboardReportData;
	USB_KeyboardReport_Data_t        KeyboardReportData;
	bool                             SendReport = false;

	/* Create the next keyboard report for transmission to the host */
	CreateKeyboardReport(&KeyboardReportData);

	/* Check if the idle period is set and has elapsed */
	if (IdleCount && (!(IdleMSRemaining)))
	{
		/* Reset the idle time remaining counter */
		IdleMSRemaining = IdleCount;

		/* Idle period is set and has elapsed, must send a report to the host */
		SendReport = true;
	}
	else
	{
		/* Check to see if the report data has changed - if so a report MUST be sent */
		SendReport = (memcmp(&PrevKeyboardReportData, &KeyboardReportData, sizeof(USB_KeyboardReport_Data_t)) != 0);
	}

	/* Select the Keyboard Report Endpoint */
	Endpoint_SelectEndpoint(KEYBOARD_IN_EPADDR);

	/* Check if Keyboard Endpoint Ready for Read/Write and if we should send a new report */
	if (Endpoint_IsReadWriteAllowed() && SendReport)
	{
		/* Save the current report data for later comparison to check for changes */
		PrevKeyboardReportData = KeyboardReportData;

		/* Write Keyboard Report Data */
		Endpoint_Write_Stream_LE(&KeyboardReportData, sizeof(KeyboardReportData), NULL);

		/* Finalize the stream transfer to send the last packet */
		Endpoint_ClearIN();
	}
}

/** Reads the next LED status report from the host from the LED data endpoint, if one has been sent. */
void HID_receiveReport(void)
{
	/* Select the Keyboard LED Report Endpoint */
	Endpoint_SelectEndpoint(KEYBOARD_OUT_EPADDR);

	/* Check if Keyboard LED Endpoint contains a packet */
	if (Endpoint_IsOUTReceived())
	{
		/* Check to see if the packet contains data */
		if (Endpoint_IsReadWriteAllowed())
		{
			/* Read in the LED report from the host */
			uint8_t LEDReport = Endpoint_Read_8();

			/* Process the read LED report from the host */
			ProcessLEDReport(LEDReport);
		}

		/* Handshake the OUT Endpoint - clear endpoint and ready for next report */
		Endpoint_ClearOUT();
	}
}

void HID_update() {
	// Check whether we are connected and configured
	if (USB_DeviceState != DEVICE_STATE_Configured) {
		return;
	}

	HID_sendReport();
	HID_receiveReport();
}
