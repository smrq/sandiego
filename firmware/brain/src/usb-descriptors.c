#include "defs.h"
#include "usb.h"

typedef struct {
	USB_ConfigurationDescriptor_t configuration;
	USB_InterfaceDescriptor_t     interface;
	HID_Descriptor_t              hid;
	USB_EndpointDescriptor_t      keyboardInEndpoint;
	USB_EndpointDescriptor_t      keyboardOutEndpoint;
} USB_CombinedConfigurationDescriptor_t;

enum StringDescriptorId_t {
	StringDescriptorId_Language     = 0,
	StringDescriptorId_Manufacturer = 1,
	StringDescriptorId_Product      = 2
};

/*
	Note that the key report is split into several similar items, because:

	An item field cannot span more than 4 bytes in a report. For example, a 32-bit
	item must start on a byte boundary to satisfy this condition.
		- USB HID 1.11 Specification, p. 57
*/
const u8 PROGMEM NkroKeyboardReportDescriptor[] = {
	HID_USAGE_PAGE      (8, HID_USAGE_PAGE_GENERIC_DESKTOP_CONTROLS),
	HID_USAGE           (8, HID_DESKTOP_KEYBOARD),
	HID_COLLECTION      (8, HID_COLLECTION_APPLICATION),

	HID_USAGE_PAGE      (8, HID_USAGE_PAGE_KEYBOARD_KEYPAD),
	HID_LOGICAL_MINIMUM (8, 0x00),
	HID_LOGICAL_MAXIMUM (8, 0x01),
	HID_REPORT_SIZE     (8, 0x01),
	HID_REPORT_COUNT    (8, 0x20),
	HID_USAGE_MINIMUM   (8, 0x00),
	HID_USAGE_MAXIMUM   (8, 0x1F),
	HID_INPUT           (8, HID_DATA | HID_VARIABLE | HID_ABSOLUTE),

	HID_USAGE_MINIMUM   (8, 0x20),
	HID_USAGE_MAXIMUM   (8, 0x3F),
	HID_INPUT           (8, HID_DATA | HID_VARIABLE | HID_ABSOLUTE),

	HID_USAGE_MINIMUM   (8, 0x40),
	HID_USAGE_MAXIMUM   (8, 0x5F),
	HID_INPUT           (8, HID_DATA | HID_VARIABLE | HID_ABSOLUTE),

	HID_USAGE_MINIMUM   (8, 0x60),
	HID_USAGE_MAXIMUM   (8, 0x7F),
	HID_INPUT           (8, HID_DATA | HID_VARIABLE | HID_ABSOLUTE),

	HID_USAGE_MINIMUM   (8, 0x80),
	HID_USAGE_MAXIMUM   (8, 0x9F),
	HID_INPUT           (8, HID_DATA | HID_VARIABLE | HID_ABSOLUTE),

	HID_USAGE_MINIMUM   (8, 0xA0),
	HID_USAGE_MAXIMUM   (8, 0xBF),
	HID_INPUT           (8, HID_DATA | HID_VARIABLE | HID_ABSOLUTE),

	HID_USAGE_MINIMUM   (8, 0xC0),
	HID_USAGE_MAXIMUM   (8, 0xDF),
	HID_INPUT           (8, HID_DATA | HID_VARIABLE | HID_ABSOLUTE),

	HID_USAGE_MINIMUM   (8, 0xE0),
	HID_USAGE_MAXIMUM   (8, 0xFF),
	HID_INPUT           (8, HID_DATA | HID_VARIABLE | HID_ABSOLUTE),

	HID_USAGE_PAGE      (8, HID_USAGE_PAGE_LEDS),
	HID_USAGE_MINIMUM   (8, 0x01),
	HID_USAGE_MAXIMUM   (8, 0x05),
	HID_REPORT_SIZE     (8, 0x01),
	HID_REPORT_COUNT    (8, 0x05),
	HID_OUTPUT          (8, HID_DATA | HID_VARIABLE | HID_ABSOLUTE),

	HID_REPORT_SIZE     (8, 0x03),
	HID_REPORT_COUNT    (8, 0x01),
	HID_OUTPUT          (8, HID_CONSTANT),

	HID_END_COLLECTION  (0)
};

const USB_DeviceDescriptor_t PROGMEM DeviceDescriptor = {
	.header = {
		.length         = sizeof(USB_DeviceDescriptor_t),
		.descriptorType = USB_DescriptorType_Device,
	},
	.usbVersion         = BCD(1,1,0),
	.deviceClass        = USB_Class_FromInterface,
	.deviceSubClass     = USB_Subclass_FromInterface,
	.deviceProtocol     = USB_Protocol_FromInterface,
	.maxPacketSize      = USB_ENDPOINT_CONTROL_SIZE,
	.vendorId           = USB_VENDOR_ID,
	.productId          = USB_PRODUCT_ID,
	.deviceVersion      = USB_RELEASE_NUMBER,
	.manufacturerString = StringDescriptorId_Manufacturer,
	.productString      = StringDescriptorId_Product,
	.serialNumberString = USB_NO_STRING_DESCRIPTOR,
	.numConfigurations  = USB_CONFIGURATION_COUNT
};

const USB_CombinedConfigurationDescriptor_t PROGMEM ConfigurationDescriptor = {
	.configuration = {
		.header = {
			.length          = sizeof(USB_ConfigurationDescriptor_t),
			.descriptorType  = USB_DescriptorType_Configuration
		},
		.totalLength         = sizeof(USB_CombinedConfigurationDescriptor_t),
		.numInterfaces       = USB_INTERFACE_COUNT,
		.configurationValue  = USB_CONFIGURATION_KEYBOARD,
		.configurationString = USB_NO_STRING_DESCRIPTOR,
		.attributes          = (USB_CONFIGURATION_ATTRIBUTES_RESERVED | USB_CONFIGURATION_ATTRIBUTES_SELF_POWERED),
		.maxPower            = USB_MAX_POWER_mA(100) // I *definitely* promise not to use more than 100mA... per LED^H^H^H^H^H^H^H
	},

	.interface = {
		.header = {
			.length         = sizeof(USB_InterfaceDescriptor_t),
			.descriptorType = USB_DescriptorType_Interface
		},
		.interfaceNumber    = USB_INTERFACE_KEYBOARD,
		.alternateSetting   = 0,
		.numEndpoints       = USB_ENDPOINT_COUNT,
		.interfaceClass     = USB_Class_HID,
		.interfaceSubClass  = HID_Subclass_Boot,
		.interfaceProtocol  = HID_Protocol_Keyboard,
		.interfaceString    = USB_NO_STRING_DESCRIPTOR
	},

	.hid = {
		.header = {
			.length         = sizeof(HID_Descriptor_t),
			.descriptorType = HID_DescriptorType_HID,
		},
		.hidVersion         = BCD(1,1,1),
		.countryCode        = HID_CountryCode_NotSupported,
		.numDescriptors     = 1,
		.descriptorType     = HID_DescriptorType_Report,
		.descriptorLength   = sizeof(NkroKeyboardReportDescriptor)
	},

	.keyboardInEndpoint = {
		.header = {
			.length         = sizeof(USB_EndpointDescriptor_t),
			.descriptorType = USB_DescriptorType_Endpoint
		},
		.endpointAddress    = USB_ENDPOINT_KEYBOARD_IN | USB_ENDPOINT_ADDRESS_DIRECTION_IN,
		.attributes         = USB_ENDPOINT_ATTRIBUTES_INTERRUPT,
		.maxPacketSize      = USB_ENDPOINT_KEYBOARD_SIZE,
		.interval           = 5
	},

	.keyboardOutEndpoint = {
		.header = {
			.length         = sizeof(USB_EndpointDescriptor_t),
			.descriptorType = USB_DescriptorType_Endpoint
		},
		.endpointAddress    = USB_ENDPOINT_KEYBOARD_OUT | USB_ENDPOINT_ADDRESS_DIRECTION_OUT,
		.attributes         = USB_ENDPOINT_ATTRIBUTES_INTERRUPT,
		.maxPacketSize      = USB_ENDPOINT_KEYBOARD_SIZE,
		.interval           = 5
	}
};

const USB_LanguageDescriptor_t      PROGMEM LanguageDescriptor     = USB_LANGUAGE_DESCRIPTOR(USB_LANGUAGE_ID_ENGLISH_US);
const USB_UnicodeStringDescriptor_t PROGMEM ManufacturerDescriptor = USB_UNICODE_STRING_DESCRIPTOR(L"smrq");
const USB_UnicodeStringDescriptor_t PROGMEM ProductDescriptor      = USB_UNICODE_STRING_DESCRIPTOR(L"Sandiego Keyboard");

bool USB_getDescriptor(u8 type, u8 number, const void **address, u16 *size) {
	switch (type) {
		case USB_DescriptorType_Device:
			*address = &DeviceDescriptor;
			*size = sizeof(DeviceDescriptor);
			return true;

		case USB_DescriptorType_Configuration:
			*address = &ConfigurationDescriptor;
			*size = sizeof(ConfigurationDescriptor);
			return true;

		case USB_DescriptorType_String:
			switch (number) {
				case StringDescriptorId_Language:
					*address = &LanguageDescriptor;
					*size = sizeof(LanguageDescriptor);
					return true;

				case StringDescriptorId_Manufacturer:
					*address = &ManufacturerDescriptor;
					*size = sizeof(ManufacturerDescriptor);
					return true;

				case StringDescriptorId_Product:
					*address = &ProductDescriptor;
					*size = sizeof(ProductDescriptor);
					return true;

				default:
					return false;
			}

		case HID_DescriptorType_HID:
			*address = &ConfigurationDescriptor.hid;
			*size = sizeof(ConfigurationDescriptor.hid);
			return true;

		case HID_DescriptorType_Report:
			*address = &NkroKeyboardReportDescriptor;
			*size = sizeof(NkroKeyboardReportDescriptor);
			return true;

		default:
			return false;
	}
}
