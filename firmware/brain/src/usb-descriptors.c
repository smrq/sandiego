#include <LUFA/Drivers/USB/USB.h>
#include "defs.h"
#include "usb-descriptors.h"

typedef struct {
	USB_Descriptor_Configuration_Header_t Config;

	// Keyboard HID Interface
	USB_Descriptor_Interface_t HID_Interface;
	USB_HID_Descriptor_HID_t   HID_KeyboardHID;
	USB_Descriptor_Endpoint_t  HID_ReportINEndpoint;
	USB_Descriptor_Endpoint_t  HID_ReportOUTEndpoint;
} USB_Descriptor_Configuration_t;

enum InterfaceDescriptors_t {
	INTERFACE_ID_Keyboard = 0
};

enum StringDescriptors_t {
	STRING_ID_Language     = 0,
	STRING_ID_Manufacturer = 1,
	STRING_ID_Product      = 2
};

const USB_Descriptor_HIDReport_Datatype_t PROGMEM BootKeyboardReportDescriptor[] = {
	HID_RI_USAGE_PAGE(8, 0x01), /* Generic Desktop */
	HID_RI_USAGE(8, 0x06), /* Keyboard */
	HID_RI_COLLECTION(8, 0x01), /* Application */

		HID_RI_USAGE_PAGE(8, 0x07), /* Key Codes */
		HID_RI_USAGE_MINIMUM(8, 0xE0), /* Keyboard Left Control */
		HID_RI_USAGE_MAXIMUM(8, 0xE7), /* Keyboard Right GUI */
		HID_RI_LOGICAL_MINIMUM(8, 0x00),
		HID_RI_LOGICAL_MAXIMUM(8, 0x01),
		HID_RI_REPORT_SIZE(8, 0x01),
		HID_RI_REPORT_COUNT(8, 0x08),
		HID_RI_INPUT(8, HID_IOF_DATA | HID_IOF_VARIABLE | HID_IOF_ABSOLUTE),

		HID_RI_REPORT_COUNT(8, 0x01),
		HID_RI_REPORT_SIZE(8, 0x08),
		HID_RI_INPUT(8, HID_IOF_CONSTANT),

		HID_RI_USAGE_PAGE(8, 0x08), /* LEDs */
		HID_RI_USAGE_MINIMUM(8, 0x01), /* Num Lock */
		HID_RI_USAGE_MAXIMUM(8, 0x05), /* Kana */
		HID_RI_REPORT_COUNT(8, 0x05),
		HID_RI_REPORT_SIZE(8, 0x01),
		HID_RI_OUTPUT(8, HID_IOF_DATA | HID_IOF_VARIABLE | HID_IOF_ABSOLUTE | HID_IOF_NON_VOLATILE),

		HID_RI_REPORT_COUNT(8, 0x01),
		HID_RI_REPORT_SIZE(8, 0x03),
		HID_RI_OUTPUT(8, HID_IOF_CONSTANT),

		HID_RI_LOGICAL_MINIMUM(8, 0x00),
		HID_RI_LOGICAL_MAXIMUM(8, 0x65),
		HID_RI_USAGE_PAGE(8, 0x07), /* Keyboard */
		HID_RI_USAGE_MINIMUM(8, 0x00), /* Reserved (no event indicated) */
		HID_RI_USAGE_MAXIMUM(8, 0x65), /* Keyboard Application */
		HID_RI_REPORT_COUNT(8, 0x06),
		HID_RI_REPORT_SIZE(8, 0x08),
		HID_RI_INPUT(8, HID_IOF_DATA | HID_IOF_ARRAY | HID_IOF_ABSOLUTE),
	HID_RI_END_COLLECTION(0)
};

const USB_Descriptor_HIDReport_Datatype_t PROGMEM NKROKeyboardReportDescriptor[] = {
	HID_RI_USAGE_PAGE(8, 0x01), /* Generic Desktop */
	HID_RI_USAGE(8, 0x06), /* Keyboard */
	HID_RI_COLLECTION(8, 0x01), /* Application */
		HID_RI_USAGE_PAGE(8, 0x07), /* Key Codes */
		HID_RI_LOGICAL_MINIMUM(8, 0x00),
		HID_RI_LOGICAL_MAXIMUM(8, 0x01),
		HID_RI_REPORT_SIZE(8, 0x01),
		HID_RI_REPORT_COUNT(8, 0x20),
			HID_RI_USAGE_MINIMUM(8, 0x00),
			HID_RI_USAGE_MAXIMUM(8, 0x1F),
			HID_RI_INPUT(8, HID_IOF_DATA | HID_IOF_VARIABLE | HID_IOF_ABSOLUTE),
			HID_RI_USAGE_MINIMUM(8, 0x20),
			HID_RI_USAGE_MAXIMUM(8, 0x3F),
			HID_RI_INPUT(8, HID_IOF_DATA | HID_IOF_VARIABLE | HID_IOF_ABSOLUTE),
			HID_RI_USAGE_MINIMUM(8, 0x40),
			HID_RI_USAGE_MAXIMUM(8, 0x5F),
			HID_RI_INPUT(8, HID_IOF_DATA | HID_IOF_VARIABLE | HID_IOF_ABSOLUTE),
			HID_RI_USAGE_MINIMUM(8, 0x60),
			HID_RI_USAGE_MAXIMUM(8, 0x7F),
			HID_RI_INPUT(8, HID_IOF_DATA | HID_IOF_VARIABLE | HID_IOF_ABSOLUTE),
			HID_RI_USAGE_MINIMUM(8, 0x80),
			HID_RI_USAGE_MAXIMUM(8, 0x9F),
			HID_RI_INPUT(8, HID_IOF_DATA | HID_IOF_VARIABLE | HID_IOF_ABSOLUTE),
			HID_RI_USAGE_MINIMUM(8, 0xA0),
			HID_RI_USAGE_MAXIMUM(8, 0xBF),
			HID_RI_INPUT(8, HID_IOF_DATA | HID_IOF_VARIABLE | HID_IOF_ABSOLUTE),
			HID_RI_USAGE_MINIMUM(8, 0xC0),
			HID_RI_USAGE_MAXIMUM(8, 0xDF),
			HID_RI_INPUT(8, HID_IOF_DATA | HID_IOF_VARIABLE | HID_IOF_ABSOLUTE),
			HID_RI_USAGE_MINIMUM(8, 0xE0),
			HID_RI_USAGE_MAXIMUM(8, 0xFF),
			HID_RI_INPUT(8, HID_IOF_DATA | HID_IOF_VARIABLE | HID_IOF_ABSOLUTE),
		HID_RI_USAGE_PAGE(8, 0x08), /* LEDs */
		HID_RI_USAGE_MINIMUM(8, 0x01), /* Num Lock */
		HID_RI_USAGE_MAXIMUM(8, 0x05), /* Kana */
		HID_RI_REPORT_COUNT(8, 0x05),
		HID_RI_REPORT_SIZE(8, 0x01),
		HID_RI_OUTPUT(8, HID_IOF_DATA | HID_IOF_VARIABLE | HID_IOF_ABSOLUTE | HID_IOF_NON_VOLATILE),
		HID_RI_REPORT_COUNT(8, 0x01),
		HID_RI_REPORT_SIZE(8, 0x03),
		HID_RI_OUTPUT(8, HID_IOF_CONSTANT),
	HID_RI_END_COLLECTION(0)
};

const USB_Descriptor_Device_t PROGMEM DeviceDescriptor = {
	.Header = {
		.Size = sizeof(USB_Descriptor_Device_t),
		.Type = DTYPE_Device
	},

	.USBSpecification       = VERSION_BCD(1,1,0),
	.Class                  = USB_CSCP_NoDeviceClass,
	.SubClass               = USB_CSCP_NoDeviceSubclass,
	.Protocol               = USB_CSCP_NoDeviceProtocol,

	.Endpoint0Size          = FIXED_CONTROL_ENDPOINT_SIZE,

	.VendorID               = VENDOR_ID,
	.ProductID              = PRODUCT_ID,
	.ReleaseNumber          = RELEASE_NUMBER,

	.ManufacturerStrIndex   = STRING_ID_Manufacturer,
	.ProductStrIndex        = STRING_ID_Product,
	.SerialNumStrIndex      = NO_DESCRIPTOR,

	.NumberOfConfigurations = FIXED_NUM_CONFIGURATIONS
};

const USB_Descriptor_Configuration_t PROGMEM ConfigurationDescriptor = {
	.Config = {
		.Header = {
			.Size = sizeof(USB_Descriptor_Configuration_Header_t),
			.Type = DTYPE_Configuration
		},

		.TotalConfigurationSize = sizeof(USB_Descriptor_Configuration_t),
		.TotalInterfaces        = 1,

		.ConfigurationNumber    = 1,
		.ConfigurationStrIndex  = NO_DESCRIPTOR,

		.ConfigAttributes       = (USB_CONFIG_ATTR_RESERVED | USB_CONFIG_ATTR_SELFPOWERED),

		// I *definitely* promise not to use more than 100mA... per LED^H^H^H^H^H^H^H
		// I think this same lie is in every single USB-based Arduino project.
		.MaxPowerConsumption    = USB_CONFIG_POWER_MA(100)
	},

	.HID_Interface = {
		.Header = {
			.Size = sizeof(USB_Descriptor_Interface_t),
			.Type = DTYPE_Interface
		},

		.InterfaceNumber        = INTERFACE_ID_Keyboard,
		.AlternateSetting       = 0x00,

		.TotalEndpoints         = 2,

		.Class                  = HID_CSCP_HIDClass,
		.SubClass               = HID_CSCP_BootSubclass,
		.Protocol               = HID_CSCP_KeyboardBootProtocol,

		.InterfaceStrIndex      = NO_DESCRIPTOR
	},

	.HID_KeyboardHID = {
		.Header = {
			.Size = sizeof(USB_HID_Descriptor_HID_t),
			.Type = HID_DTYPE_HID
		},

		.HIDSpec                = VERSION_BCD(1,1,1),
		.CountryCode            = 0x00,
		.TotalReportDescriptors = 1,
		.HIDReportType          = HID_DTYPE_Report,
		.HIDReportLength        = sizeof(NKROKeyboardReportDescriptor)
	},

	.HID_ReportINEndpoint = {
		.Header = {
			.Size = sizeof(USB_Descriptor_Endpoint_t),
			.Type = DTYPE_Endpoint
		},

		.EndpointAddress        = KEYBOARD_IN_EPADDR,
		.Attributes             = (EP_TYPE_INTERRUPT | ENDPOINT_ATTR_NO_SYNC | ENDPOINT_USAGE_DATA),
		.EndpointSize           = KEYBOARD_EPSIZE,
		.PollingIntervalMS      = 0x05
	},

	.HID_ReportOUTEndpoint = {
		.Header = {
			.Size = sizeof(USB_Descriptor_Endpoint_t),
			.Type = DTYPE_Endpoint
		},

		.EndpointAddress        = KEYBOARD_OUT_EPADDR,
		.Attributes             = (EP_TYPE_INTERRUPT | ENDPOINT_ATTR_NO_SYNC | ENDPOINT_USAGE_DATA),
		.EndpointSize           = KEYBOARD_EPSIZE,
		.PollingIntervalMS      = 0x05
	}
};

const USB_Descriptor_String_t PROGMEM LanguageString = USB_STRING_DESCRIPTOR_ARRAY(LANGUAGE_ID_ENG);
const USB_Descriptor_String_t PROGMEM ManufacturerString = USB_STRING_DESCRIPTOR(L"smrq");
const USB_Descriptor_String_t PROGMEM ProductString = USB_STRING_DESCRIPTOR(L"Sandiego Keyboard");

u16 CALLBACK_USB_GetDescriptor(const u16 wValue, UNUSED const u16 wIndex, const void** const descriptorAddress) {
	const u8 descriptorType = (wValue >> 8) & 0xFF;
	const u8 descriptorNumber = (wValue >> 0) & 0xFF;

	const void *address = NULL;
	u16 size = NO_DESCRIPTOR;

	switch (descriptorType) {
		case DTYPE_Device:
			address = &DeviceDescriptor;
			size = sizeof(USB_Descriptor_Device_t);
			break;

		case DTYPE_Configuration:
			address = &ConfigurationDescriptor;
			size = sizeof(USB_Descriptor_Configuration_t);
			break;

		case DTYPE_String:
			switch (descriptorNumber) {
				case STRING_ID_Language:
					address = &LanguageString;
					size = pgm_read_byte(&LanguageString.Header.Size);
					break;
				case STRING_ID_Manufacturer:
					address = &ManufacturerString;
					size = pgm_read_byte(&ManufacturerString.Header.Size);
					break;
				case STRING_ID_Product:
					address = &ProductString;
					size = pgm_read_byte(&ProductString.Header.Size);
					break;
			}

			break;

		case HID_DTYPE_HID:
			address = &ConfigurationDescriptor.HID_KeyboardHID;
			size = sizeof(USB_HID_Descriptor_HID_t);
			break;

		case HID_DTYPE_Report:
			address = &NKROKeyboardReportDescriptor;
			size = sizeof(NKROKeyboardReportDescriptor);
			break;
	}

	*descriptorAddress = address;
	return size;
}
