#include "defs.h"
#include "./usb.h"

/*
	Note for all control requests:

	The Status stage of a control transfer is the last operation in the sequence. A Status stage is delineated by a
	change in direction of data flow from the previous stage and always uses a DATA1 PID. If, for example,
	the Data stage consists of OUTs, the status is a single IN transaction. If the control sequence has no Data
	stage, then it consists of a Setup stage followed by a Status stage consisting of an IN transaction.
		- USB 1.1 Specification, section 8.5.2 Control Transfers, p. 165
*/

/* USB 1.1 Specification, section 9.4.6 Set Address, p. 192 */
local bool USB_handleSetAddress(u8 value) {
	/*
		If the specified device address is greater than 127 (...) then the behavior of the device is not specified.
		Device response to SetAddress() with a value of 0 is undefined.
			- USB 1.1 Specification, section 9.4.6 Set Address, p. 192
	*/
	u8 deviceAddress = value & 0x7F;

	/*
		The USB device address is set up according to the USB protocol:
		- the USB device, after power-up, responds at address 0
		- the host sends a SETUP command (SET_ADDRESS(addr))
		- the firmware handles this request, and records that address in UADD, but keep ADDEN cleared
		- the USB device firmware sends an IN command of 0 bytes (IN 0 Zero Length Packet)
		- then, the firmware can enable the USB device address by setting ADDEN. The only accepted address by
		  the controller is the one stored in UADD.
		ADDEN and UADD shall not be written at the same time.
			- ATmega32U4 datasheet, section 22.7 Address Setup, p. 272

		The USB device does not change its device address until after the Status stage of this request is
		completed successfully. Note that this is a difference between this request and all other requests.
			- USB 1.1 Specification, section 9.4.6 Set Address, p. 192
	*/
	USB_setAddress(deviceAddress);

	if (!USB_waitForINReady()) return false;
	USB_sendIN();

	if (!USB_waitForINReady()) return false;
	USB_enableAddress();
	USB_DEVICE_STATE = USB_DeviceState_Address;

	return true;
}

/* USB 1.1 Specification, section 9.4.2 Get Configuration, p. 189 */
local bool USB_handleGetConfiguration() {
	/*
		Default state: Device behavior when this request is received while the device is in the Default state is not specified.
		Address state: The value zero shall be returned.
		Configured state: The non-zero bConfigurationNumber of the current configuration shall be returned.
			- USB 1.1 Specification, section 9.4.2 Get Configuration, p. 189
	*/
	u8 value;
	switch (USB_DEVICE_STATE) {
		case USB_DeviceState_Address:
			value = 0;
			break;

		case USB_DeviceState_Configured:
			value = USB_ConfigurationNumber;
			break;

		default:
			return false;
	}

	if (!USB_waitForINReady()) return false;
	USB_writeByteToEndpoint(value);
	USB_sendIN();

	if (!USB_waitForOUTReady()) return false;
	USB_clearOUT();

	return true;
}

/* USB 1.1 Specification, section 9.4.7 Set Configuration, p. 193 */
local bool USB_handleSetConfiguration(u8 value) {
	/*
		The lower byte of the wValue field specifies the desired configuration. This configuration value must be
		zero or match a configuration value from a configuration descriptor. If the configuration value is zero, the
		device is placed in its Address state.

		Default state: Device behavior when this request is received while the device is in the Default state
			is not specified.
		Address state: If the specified configuration value is zero, then the device remains in the Address
			state. If the specified configuration value matches the configuration value from a
			configuration descriptor, then that configuration is selected and the device enters the
			Configured state. Otherwise, the device responds with a Request Error.
		Configured state: If the specified configuration value is zero, then the device enters the Address state.
			If the specified configuration value matches the configuration value from a
			configuration descriptor, then that configuration is selected and the device remains in
			the Configured state. Otherwise, the device responds with a Request Error.
			- USB 1.1 Specification, section 9.4.7 Set Configuration, p. 193
	*/
	if (value > USB_CONFIGURATION_COUNT) {
		return false;
	}

	USB_ConfigurationNumber = value;

	if (value) {
		USB_DEVICE_STATE = USB_DeviceState_Configured;
		USB_configureEndpoint(USB_ENDPOINT_KEYBOARD_IN,
			USB_CFG0X_ENDPOINT_TYPE_INTERRUPT | USB_CFG0X_ENDPOINT_DIRECTION_IN,
			USB_CFG1X_ENDPOINT_SIZE(USB_ENDPOINT_KEYBOARD_SIZE));
		USB_configureEndpoint(USB_ENDPOINT_KEYBOARD_OUT,
			USB_CFG0X_ENDPOINT_TYPE_INTERRUPT | USB_CFG0X_ENDPOINT_DIRECTION_OUT,
			USB_CFG1X_ENDPOINT_SIZE(USB_ENDPOINT_KEYBOARD_SIZE));
		USB_selectEndpoint(USB_ENDPOINT_CONTROL);
		UDIEN |= _BV(SOFE);
	} else {
		USB_DEVICE_STATE = USB_DeviceState_Address;
		UDIEN &= _BV(SOFE);
	}

	if (!USB_waitForINReady()) return false;
	USB_sendIN();

	return true;
}

/* USB 1.1 Specification, section 9.4.3 Get Descriptor, p. 189 */
local bool USB_handleGetDescriptor(u16 value, u8 length) {
	u8 descriptorType = (value >> 8) & 0xFF;
	u8 descriptorIndex = (value >> 0) & 0xFF;

	const void *descriptorAddress;
	u16 descriptorSize;
	if (!USB_getDescriptor(descriptorType, descriptorIndex, &descriptorAddress, &descriptorSize)) {
		return false;
	}

	/*
		If the descriptor is longer than the wLength field, only the initial bytes of the descriptor are returned.
			- USB 1.1 Specification, section 9.4.3 Get Descriptor, p. 189
	*/
	if (descriptorSize > length) {
		descriptorSize = length;
	}

	if (!USB_transferData(descriptorAddress, descriptorSize, true, USB_ENDPOINT_CONTROL_SIZE)) {
		return false;
	}

	if (!USB_waitForOUTReady()) return false;
	USB_clearOUT();

	return true;
}

/* USB 1.1 Specification, section 9.4.1 Clear Feature, p. 188 */
local bool USB_handleClearDeviceFeature(u8 value) {
	switch (value) {
		case USB_FeatureSelector_DeviceRemoteWakeup:
			USB_RemoteWakeupEnabled = false;
			break;

		default:
			return false;
	}

	if (!USB_waitForINReady()) return false;
	USB_sendIN();

	return true;
}

/* USB 1.1 Specification, section 9.4.9 Set Feature, p. 194 */
local bool USB_handleSetDeviceFeature(u8 value) {
	switch (value) {
		case USB_FeatureSelector_DeviceRemoteWakeup:
			USB_RemoteWakeupEnabled = true;
			break;

		default:
			return false;
	}

	if (!USB_waitForINReady()) return false;
	USB_sendIN();

	return true;
}

/* USB 1.1 Specification, section 9.4.1 Clear Feature, p. 188 */
local bool USB_handleClearEndpointFeature(u8 value, u8 index) {
	u8 endpointNumber = index & USB_REQUEST_INDEX_ENDPOINT_NUMBER_MASK;

	switch (value) {
		case USB_FeatureSelector_EndpointHalt:
			if (endpointNumber == USB_ENDPOINT_CONTROL) {
				return false;
			}

			if (endpointNumber >= USB_ENDPOINT_COUNT) {
				return false;
			}

			USB_selectEndpoint(endpointNumber);
			if (USB_isEndpointEnabled()) {
				USB_clearEndpointStall();
				USB_resetEndpoint(endpointNumber);

				/*
					For endpoints using data toggle, regardless of whether an endpoint has the Halt feature set, a
					ClearFeature(ENDPOINT_HALT) request always results in the data toggle being reinitialized to DATA0.
						- USB 1.1 Specification, p. 192
				*/
				USB_resetEndpointDataToggle();
			}
			USB_selectEndpoint(USB_ENDPOINT_CONTROL);
			break;

		default:
			return false;
	}

	if (!USB_waitForINReady()) return false;
	USB_sendIN();

	return true;
}

/* USB 1.1 Specification, section 9.4.9 Set Feature, p. 194 */
local bool USB_handleSetEndpointFeature(u8 value, u8 index) {
	u8 endpointNumber = index & USB_REQUEST_INDEX_ENDPOINT_NUMBER_MASK;

	switch (value) {
		case USB_FeatureSelector_EndpointHalt:
			if (endpointNumber == USB_ENDPOINT_CONTROL) {
				return false;
			}

			if (endpointNumber >= USB_ENDPOINT_COUNT) {
				return false;
			}

			USB_selectEndpoint(endpointNumber);
			if (USB_isEndpointEnabled()) {
				USB_stallEndpoint();
			}
			USB_selectEndpoint(USB_ENDPOINT_CONTROL);

			break;

		default:
			return false;
	}

	if (!USB_waitForINReady()) return false;
	USB_sendIN();

	return true;
}

/* USB 1.1 Specification, section 9.4.5 Get Status, p. 190 */
local bool USB_handleGetDeviceStatus() {
	u8 status = (USB_RemoteWakeupEnabled ? _BV(1) : 0);

	if (!USB_waitForINReady()) return false;
	USB_writeByteToEndpoint(status);
	USB_writeByteToEndpoint(0);
	USB_sendIN();

	if (!USB_waitForOUTReady()) return false;
	USB_clearOUT();

	return true;
}

/* USB 1.1 Specification, section 9.4.5 Get Status, p. 190 */
local bool USB_handleGetEndpointStatus(u8 index) {
	u8 endpointNumber = index & USB_REQUEST_INDEX_ENDPOINT_NUMBER_MASK;

	if (endpointNumber >= USB_ENDPOINT_COUNT) {
		return false;
	}

	USB_selectEndpoint(endpointNumber);
	u8 status = USB_isEndpointStalled() ? _BV(0) : 0;
	USB_selectEndpoint(USB_ENDPOINT_CONTROL);

	if (!USB_waitForINReady()) return false;
	USB_writeByteToEndpoint(status);
	USB_writeByteToEndpoint(0);
	USB_sendIN();

	if (!USB_waitForOUTReady()) return false;
	USB_clearOUT();

	return true;
}

/* USB HID 1.11 Specification, section 7.2.1 Get_Report Request, p. 51 */
local bool USB_handleGetReport(u8 length) {
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

	if (reportSize > length) {
		reportSize = length;
	}

	USB_transferData(&report, reportSize, false, USB_ENDPOINT_CONTROL_SIZE);

	if (!USB_waitForOUTReady()) return false;
	USB_clearOUT();

	return true;
}

/* USB HID 1.11 Specification, section 7.2.2 Set_Report Request, p. 52 */
local bool USB_handleSetReport() {
	if (!USB_waitForOUTReady()) return false;

	USB_ledReport_t report = USB_readByteFromEndpoint();
	USB_clearOUT();

	if (!USB_waitForINReady()) return false;
	USB_sendIN();

	processLEDReport(report);

	return true;
}

/* USB HID 1.11 Specification, section 7.2.3 Get_Idle Request, p. 52 */
local bool USB_handleGetIdle() {
	if (!USB_waitForINReady()) return false;
	USB_writeByteToEndpoint(USB_IdleTimeoutDuration >> 2);
	USB_sendIN();

	if (!USB_waitForOUTReady()) return false;
	USB_clearOUT();

	return true;
}

/* USB HID 1.11 Specification, section 7.2.4 Set_Idle Request, p. 52 */
local bool USB_handleSetIdle(u8 value) {
	USB_IdleTimeoutDuration = (value & 0xFF00) >> 6;

	if (!USB_waitForINReady()) return false;
	USB_sendIN();

	return true;
}

/* USB HID 1.11 Specification, section 7.2.5 Get_Protocol Request, p. 54 */
local bool USB_handleGetProtocol() {
	if (!USB_waitForINReady()) return false;
	USB_writeByteToEndpoint(USB_UsingReportProtocol);
	USB_sendIN();

	if (!USB_waitForOUTReady()) return false;
	USB_clearOUT();

	return true;
}

/* USB HID 1.11 Specification, section 7.2.6 Set_Protocol Request, p. 54 */
local bool USB_handleSetProtocol(u8 value) {
	USB_UsingReportProtocol = value;

	if (!USB_waitForINReady()) return false;
	USB_sendIN();

	return true;
}

local void USB_handleControlRequest(USB_DeviceRequest_t *controlRequest) {
	bool handled = false;

	// These flags are soooo long.
	#define ATTR_D2H_STD_DEV (USB_REQUEST_ATTRIBUTE_DIRECTION_DEVICE_TO_HOST | USB_REQUEST_ATTRIBUTE_TYPE_STANDARD | USB_REQUEST_ATTRIBUTE_RECIPIENT_DEVICE)
	#define ATTR_H2D_STD_DEV (USB_REQUEST_ATTRIBUTE_DIRECTION_HOST_TO_DEVICE | USB_REQUEST_ATTRIBUTE_TYPE_STANDARD | USB_REQUEST_ATTRIBUTE_RECIPIENT_DEVICE)
	#define ATTR_D2H_STD_END (USB_REQUEST_ATTRIBUTE_DIRECTION_DEVICE_TO_HOST | USB_REQUEST_ATTRIBUTE_TYPE_STANDARD | USB_REQUEST_ATTRIBUTE_RECIPIENT_ENDPOINT)
	#define ATTR_H2D_STD_END (USB_REQUEST_ATTRIBUTE_DIRECTION_HOST_TO_DEVICE | USB_REQUEST_ATTRIBUTE_TYPE_STANDARD | USB_REQUEST_ATTRIBUTE_RECIPIENT_ENDPOINT)
	#define ATTR_D2H_STD_INT (USB_REQUEST_ATTRIBUTE_DIRECTION_DEVICE_TO_HOST | USB_REQUEST_ATTRIBUTE_TYPE_STANDARD | USB_REQUEST_ATTRIBUTE_RECIPIENT_INTERFACE)
	#define ATTR_D2H_CLS_INT (USB_REQUEST_ATTRIBUTE_DIRECTION_DEVICE_TO_HOST | USB_REQUEST_ATTRIBUTE_TYPE_CLASS | USB_REQUEST_ATTRIBUTE_RECIPIENT_INTERFACE)
	#define ATTR_H2D_CLS_INT (USB_REQUEST_ATTRIBUTE_DIRECTION_HOST_TO_DEVICE | USB_REQUEST_ATTRIBUTE_TYPE_CLASS | USB_REQUEST_ATTRIBUTE_RECIPIENT_INTERFACE)
	#define REQUEST(req, type) (((req) << 8) | (type))

	switch (*(u16 *)controlRequest) {
		/* USB 1.1 Specification, section 9.4.1 Clear Feature, p. 188 */
		case REQUEST(USB_RequestCode_ClearFeature, ATTR_H2D_STD_DEV):
			handled = USB_handleClearDeviceFeature(controlRequest->value);
			break;

		/* USB 1.1 Specification, section 9.4.1 Clear Feature, p. 188 */
		case REQUEST(USB_RequestCode_ClearFeature, ATTR_H2D_STD_END):
			handled = USB_handleClearEndpointFeature(controlRequest->value, controlRequest->index);
			break;

		/* USB 1.1 Specification, section 9.4.2 Get Configuration, p. 189 */
		case REQUEST(USB_RequestCode_GetConfiguration, ATTR_D2H_STD_DEV):
			handled = USB_handleGetConfiguration();
			break;

		/* USB 1.1 Specification, section 9.4.3 Get Descriptor, p. 189 */
		case REQUEST(USB_RequestCode_GetDescriptor, ATTR_D2H_STD_DEV):
		case REQUEST(USB_RequestCode_GetDescriptor, ATTR_D2H_STD_INT):
			handled = USB_handleGetDescriptor(controlRequest->value, controlRequest->length);
			break;

		/* USB 1.1 Specification, section 9.4.5 Get Status, p. 190 */
		case REQUEST(USB_RequestCode_GetStatus, ATTR_D2H_STD_DEV):
			handled = USB_handleGetDeviceStatus();
			break;

		/* USB 1.1 Specification, section 9.4.5 Get Status, p. 190 */
		case REQUEST(USB_RequestCode_GetStatus, ATTR_D2H_STD_END):
			handled = USB_handleGetEndpointStatus(controlRequest->index);
			break;

		/* USB 1.1 Specification, section 9.4.6 Set Address, p. 192 */
		case REQUEST(USB_RequestCode_SetAddress, ATTR_H2D_STD_DEV):
			handled = USB_handleSetAddress(controlRequest->value);
			break;

		// /* USB 1.1 Specification, section 9.4.7 Set Configuration, p. 193 */
		case REQUEST(USB_RequestCode_SetConfiguration, ATTR_H2D_STD_DEV):
			handled = USB_handleSetConfiguration(controlRequest->value);
			break;

		/* USB 1.1 Specification, section 9.4.9 Set Feature, p. 194 */
		case REQUEST(USB_RequestCode_SetFeature, ATTR_H2D_STD_DEV):
			handled = USB_handleSetDeviceFeature(controlRequest->value);
			break;

		/* USB 1.1 Specification, section 9.4.9 Set Feature, p. 194 */
		case REQUEST(USB_RequestCode_SetFeature, ATTR_H2D_STD_END):
			handled = USB_handleSetEndpointFeature(controlRequest->value, controlRequest->index);
			break;

		/* USB HID 1.11 Specification, section 7.2.1 Get_Report Request, p. 51 */
		case REQUEST(HID_RequestCode_GetReport, ATTR_D2H_CLS_INT):
			handled = USB_handleGetReport(controlRequest->length);
			break;

		/* USB HID 1.11 Specification, section 7.2.2 Set_Report Request, p. 52 */
		case REQUEST(HID_RequestCode_SetReport, ATTR_H2D_CLS_INT):
			handled = USB_handleSetReport();
			break;

		/* USB HID 1.11 Specification, section 7.2.3 Get_Idle Request, p. 52 */
		case REQUEST(HID_RequestCode_GetIdle, ATTR_D2H_CLS_INT):
			handled = USB_handleGetIdle();
			break;

		/* USB HID 1.11 Specification, section 7.2.4 Set_Idle Request, p. 52 */
		case REQUEST(HID_RequestCode_SetIdle, ATTR_H2D_CLS_INT):
			handled = USB_handleSetIdle(controlRequest->value);
			break;

		/* USB HID 1.11 Specification, section 7.2.5 Get_Protocol Request, p. 54 */
		case REQUEST(HID_RequestCode_GetProtocol, ATTR_D2H_CLS_INT):
			handled = USB_handleGetProtocol();
			break;

		/* USB HID 1.11 Specification, section 7.2.6 Set_Protocol Request, p. 54 */
		case REQUEST(HID_RequestCode_SetProtocol, ATTR_H2D_CLS_INT):
			handled = USB_handleSetProtocol(controlRequest->value);
			break;

		default:
			break;
	}

	#undef ATTR_D2H_STD_DEV
	#undef ATTR_H2D_STD_DEV
	#undef ATTR_D2H_STD_END
	#undef ATTR_H2D_STD_END
	#undef ATTR_D2H_CLS_INT
	#undef ATTR_H2D_CLS_INT
	#undef REQUEST

	if (!handled)
	{
		USB_stallEndpoint();
	}
}

ISR(USB_COM_vect, ISR_BLOCK) {
	/*
		Make sure that no matter when this interrupt fires, it restores the global
		state of the USB hardware after it's finished.

		Even if it gets pre-empted by another interrupt that modifies the USB state
		during the period when interrupts are re-enabled, as long as that other
		interrupt also behaves itself, there should be no problems.
	*/

	u8 previousEndpoint = USB_getSelectedEndpoint();
	USB_selectEndpoint(USB_ENDPOINT_CONTROL);
	UEIENX &= ~_BV(RXSTPE);

	enableGlobalInterrupts();

	USB_DeviceRequest_t controlRequest;
	USB_readFromEndpoint(&controlRequest, sizeof(controlRequest));
	USB_clearSETUP();
	USB_handleControlRequest(&controlRequest);

	USB_selectEndpoint(USB_ENDPOINT_CONTROL);
	UEIENX |= _BV(RXSTPE);

	USB_selectEndpoint(previousEndpoint);
}
