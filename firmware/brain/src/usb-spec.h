#pragma once
#include "defs.h"

/* USB 1.1 Specification, section 9.1.1 Visible Device States, p. 175 */
enum USB_DeviceState_t {
	USB_DeviceState_Detached,
	// USB_DeviceState_Attached,
	USB_DeviceState_Powered,
	USB_DeviceState_Default,
	USB_DeviceState_Address,
	USB_DeviceState_Configured,
	USB_DeviceState_Suspended
};

/* USB 1.1 Specification, section 9.3 Device Requests, p. 183 */
typedef struct {
	u8 requestType;
	u8 request;
	u16 value;
	u16 index;
	u16 length;
} PACKED USB_DeviceRequest_t;

#define USB_REQUEST_ATTRIBUTE_DIRECTION_MASK           0x80
#define USB_REQUEST_ATTRIBUTE_DIRECTION_HOST_TO_DEVICE 0
#define USB_REQUEST_ATTRIBUTE_DIRECTION_DEVICE_TO_HOST 0x80

#define USB_REQUEST_ATTRIBUTE_TYPE_MASK     0x60
#define USB_REQUEST_ATTRIBUTE_TYPE_STANDARD 0
#define USB_REQUEST_ATTRIBUTE_TYPE_CLASS    0x20
#define USB_REQUEST_ATTRIBUTE_TYPE_VENDOR   0x40

#define USB_REQUEST_ATTRIBUTE_RECIPIENT_MASK      0x1F
#define USB_REQUEST_ATTRIBUTE_RECIPIENT_DEVICE    0
#define USB_REQUEST_ATTRIBUTE_RECIPIENT_INTERFACE 0x01
#define USB_REQUEST_ATTRIBUTE_RECIPIENT_ENDPOINT  0x02
#define USB_REQUEST_ATTRIBUTE_RECIPIENT_OTHER     0x03

/* USB 1.1 Specification, section 9.3.4 wIndex, p. 184 */
#define USB_REQUEST_INDEX_ENDPOINT_NUMBER_MASK    0x0003
#define USB_REQUEST_INDEX_ENDPOINT_DIRECTION_MASK 0x0080
#define USB_REQUEST_INDEX_INTERFACE_NUMBER_MASK   0x00FF

/* USB 1.1 Specification, Table 9-4. Standard Request Codes, p. 187 */
enum USB_RequestCode_t {
	USB_RequestCode_GetStatus        = 0,
	USB_RequestCode_ClearFeature     = 1,
	USB_RequestCode_SetFeature       = 3,
	USB_RequestCode_SetAddress       = 5,
	USB_RequestCode_GetDescriptor    = 6,
	USB_RequestCode_SetDescriptor    = 7,
	USB_RequestCode_GetConfiguration = 8,
	USB_RequestCode_SetConfiguration = 9,
	USB_RequestCode_GetInterface     = 10,
	USB_RequestCode_SetInterface     = 11,
	USB_RequestCode_SynchFrame       = 12
};

/* USB 1.1 Specification, Table 9-5. Descriptor Types, p. 187 */
enum USB_DescriptorType_t {
	USB_DescriptorType_Device        = 1,
	USB_DescriptorType_Configuration = 2,
	USB_DescriptorType_String        = 3,
	USB_DescriptorType_Interface     = 4,
	USB_DescriptorType_Endpoint      = 5
};

/* USB 1.1 Specification, Table 9-6. Standard Feature Selectors, p. 188 */
enum USB_FeatureSelector_t {
	USB_FeatureSelector_EndpointHalt       = 0,
	USB_FeatureSelector_DeviceRemoteWakeup = 1
};

/*
	If a device does not support string descriptors, string reference fields must
	be reset to zero to indicate no string descriptor is available.
		- USB 1.1 Specification, Section 9.5 Descriptors, p. 196
*/
#define USB_NO_STRING_DESCRIPTOR 0

/* USB 1.1 Specification, Table 9-7. Standard Device Descriptor, p. 197 */
typedef struct {
	u8 length;
	u8 descriptorType;
} PACKED USB_DescriptorHeader_t;

typedef struct {
	USB_DescriptorHeader_t header;
	u16 usbVersion;
	u8 deviceClass;
	u8 deviceSubClass;
	u8 deviceProtocol;
	u8 maxPacketSize;
	u16 vendorId;
	u16 productId;
	u16 deviceVersion;
	u8 manufacturerString;
	u8 productString;
	u8 serialNumberString;
	u8 numConfigurations;
} PACKED USB_DeviceDescriptor_t;

#define BCD(major, minor, patch) (((major & 0xFF) << 8) | ((minor & 0xF) << 4) | ((patch & 0xF) << 0))

/* USB 1.1 Specification, Table 9-8. Standard Configuration Descriptor, p. 199 */
typedef struct {
	USB_DescriptorHeader_t header;
	u16 totalLength;
	u8 numInterfaces;
	u8 configurationValue;
	u8 configurationString;
	u8 attributes;
	u8 maxPower;
} PACKED USB_ConfigurationDescriptor_t;

#define USB_CONFIGURATION_ATTRIBUTES_RESERVED      _BV(7)
#define USB_CONFIGURATION_ATTRIBUTES_SELF_POWERED  _BV(6)
#define USB_CONFIGURATION_ATTRIBUTES_REMOTE_WAKEUP _BV(5)

/*
	Expressed in 2mA units (i.e., 50 = 100mA).
		- USB 1.1 Specification, Table 9-8. Standard Configuration Descriptor, p. 199
*/
#define USB_MAX_POWER_mA(power) (power / 2)

/* USB 1.1 Specification, Table 9-9. Standard Interface Descriptor, p. 202 */
typedef struct {
	USB_DescriptorHeader_t header;
	u8 interfaceNumber;
	u8 alternateSetting;
	u8 numEndpoints;
	u8 interfaceClass;
	u8 interfaceSubClass;
	u8 interfaceProtocol;
	u8 interfaceString;
} PACKED USB_InterfaceDescriptor_t;

/* USB 1.1 Specification, Table 9-10. Standard Endpoint Descriptor, p. 203 */
typedef struct {
	USB_DescriptorHeader_t header;
	u8 endpointAddress;
	u8 attributes;
	u16 maxPacketSize;
	u8 interval;
} PACKED USB_EndpointDescriptor_t;

#define USB_ENDPOINT_ADDRESS_NUMBER_MASK    0x0F
#define USB_ENDPOINT_ADDRESS_DIRECTION_MASK 0x80
#define USB_ENDPOINT_ADDRESS_DIRECTION_IN   0x80
#define USB_ENDPOINT_ADDRESS_DIRECTION_OUT  0x00

#define USB_ENDPOINT_ATTRIBUTES_CONTROL     0
#define USB_ENDPOINT_ATTRIBUTES_ISOCHRONOUS 1
#define USB_ENDPOINT_ATTRIBUTES_BULK        2
#define USB_ENDPOINT_ATTRIBUTES_INTERRUPT   3

/* USB 1.1 Specification, Table 9-12. UNICODE String Descriptor, p. 205 */
typedef struct {
	USB_DescriptorHeader_t header;
	u16 languages[];
} PACKED USB_LanguageDescriptor_t;

typedef struct {
	USB_DescriptorHeader_t header;
	wchar_t string[];
} PACKED USB_UnicodeStringDescriptor_t;

#define USB_LANGUAGE_DESCRIPTOR(...)       { .header = { .length = sizeof(USB_DescriptorHeader_t) + sizeof((u16[]){ __VA_ARGS__ }), .descriptorType = USB_DescriptorType_String }, .languages = { __VA_ARGS__ }}
#define USB_UNICODE_STRING_DESCRIPTOR(str) { .header = { .length = sizeof(USB_DescriptorHeader_t) + sizeof(str) - sizeof(L""), .descriptorType = USB_DescriptorType_String }, .string = str }

/* http://www.usb.org/developers/docs/USB_LANGIDs.pdf */
#define USB_LANGUAGE_ID_ENGLISH_US 0x0409

/* http://www.usb.org/developers/defined_class */
enum {
	USB_Class_FromInterface = 0,
	USB_Class_HID           = 3
} USB_Class_t;

enum {
	USB_Subclass_FromInterface = 0
} USB_Subclass_t;

enum {
	USB_Protocol_FromInterface = 0
} USB_Protocol_t;

/* USB HID 1.11 Specification, section 4.2 Subclass, p. 8 */
enum {
	HID_Subclass_None = 0,
	HID_Subclass_Boot = 1
} HID_Subclass_t;

/* USB HID 1.11 Specification, section 4.2 Protocols, p. 9 */
enum {
	HID_Protocol_None     = 0,
	HID_Protocol_Keyboard = 1,
	HID_Protocol_Mouse    = 2
} HID_Protocol_t;

/* USB HID 1.11 Specification, section 6.2.1 HID Descriptor, p. 22 */
typedef struct {
	USB_DescriptorHeader_t header;
	u16 hidVersion;
	u8 countryCode;
	u8 numDescriptors;
	u8 descriptorType;
	u16 descriptorLength;
} PACKED HID_Descriptor_t;

/* USB HID 1.11 Specification, section 6.2.1 HID Descriptor, p. 23 */
enum {
	HID_CountryCode_NotSupported = 0,
	HID_CountryCode_US           = 33
} HID_CountryCode_t;

/* USB HID 1.11 Specification, section 6.2.2 Report Descriptor, p. 23 */
/* USB HID 1.11 Specification, section 6.2.2.2 Short Items, p. 26 */
#define HID_REPORT_ITEM_SIZE_0  0
#define HID_REPORT_ITEM_SIZE_8  1
#define HID_REPORT_ITEM_SIZE_16 2
#define HID_REPORT_ITEM_SIZE_32 3

#define HID_REPORT_ITEM_TYPE_MAIN   0
#define HID_REPORT_ITEM_TYPE_GLOBAL 1
#define HID_REPORT_ITEM_TYPE_LOCAL  2

#define HID_REPORT_ITEM_DATA_0(data)
#define HID_REPORT_ITEM_DATA_8(data) , (data)
#define HID_REPORT_ITEM_DATA_16(data) , ((data) >> 0 & 0xFF), ((data) >> 8 & 0xFF)
#define HID_REPORT_ITEM_DATA_32(data) , ((data) >> 0 & 0xFF), ((data) >> 8 & 0xFF), ((data) >> 16 & 0xFF), ((data) >> 24 & 0xFF)
#define HID_REPORT_ITEM_DATA(size, ...) HID_REPORT_ITEM_DATA_##size(__VA_ARGS__)

#define HID_REPORT_ITEM(tag, type, size, ...) ((tag << 4) | (type << 2) | HID_REPORT_ITEM_SIZE_##size) HID_REPORT_ITEM_DATA(size, __VA_ARGS__)

/* USB HID 1.11 Specification, section 6.2.2.4 Main Items, p. 28 */
#define HID_INPUT(size, ...)          HID_REPORT_ITEM(0x8, HID_REPORT_ITEM_TYPE_MAIN, size, __VA_ARGS__)
#define HID_OUTPUT(size, ...)         HID_REPORT_ITEM(0x9, HID_REPORT_ITEM_TYPE_MAIN, size, __VA_ARGS__)
#define HID_FEATURE(size, ...)        HID_REPORT_ITEM(0xB, HID_REPORT_ITEM_TYPE_MAIN, size, __VA_ARGS__)
#define HID_COLLECTION(size, ...)     HID_REPORT_ITEM(0xA, HID_REPORT_ITEM_TYPE_MAIN, size, __VA_ARGS__)
#define HID_END_COLLECTION(size, ...) HID_REPORT_ITEM(0xC, HID_REPORT_ITEM_TYPE_MAIN, size, __VA_ARGS__)

#define HID_DATA             (0 << 0)
#define HID_CONSTANT         (1 << 0)
#define HID_ARRAY            (0 << 1)
#define HID_VARIABLE         (1 << 1)
#define HID_ABSOLUTE         (0 << 2)
#define HID_RELATIVE         (1 << 2)
#define HID_NO_WRAP          (0 << 3)
#define HID_WRAP             (1 << 3)
#define HID_LINEAR           (0 << 4)
#define HID_NON_LINEAR       (1 << 4)
#define HID_PREFERRED_STATE  (0 << 5)
#define HID_NO_PREFERRED     (1 << 5)
#define HID_NO_NULL_POSITION (0 << 6)
#define HID_NULL_STATE       (1 << 6)
#define HID_BIT_FIELD        (0 << 8)
#define HID_BUFFERED_BYTES   (1 << 8)

#define HID_COLLECTION_PHYSICAL       0x0
#define HID_COLLECTION_APPLICATION    0x1
#define HID_COLLECTION_LOGICAL        0x2
#define HID_COLLECTION_REPORT         0x3
#define HID_COLLECTION_NAMED_ARRAY    0x4
#define HID_COLLECTION_USAGE_SWITCH   0x5
#define HID_COLLECTION_USAGE_MODIFIER 0x6

/* USB HID 1.11 Specification, section 6.2.2.7 Global Items, p. 35 */
#define HID_USAGE_PAGE(size, ...)       HID_REPORT_ITEM(0x0, HID_REPORT_ITEM_TYPE_GLOBAL, size, __VA_ARGS__)
#define HID_LOGICAL_MINIMUM(size, ...)  HID_REPORT_ITEM(0x1, HID_REPORT_ITEM_TYPE_GLOBAL, size, __VA_ARGS__)
#define HID_LOGICAL_MAXIMUM(size, ...)  HID_REPORT_ITEM(0x2, HID_REPORT_ITEM_TYPE_GLOBAL, size, __VA_ARGS__)
#define HID_PHYSICAL_MINIMUM(size, ...) HID_REPORT_ITEM(0x3, HID_REPORT_ITEM_TYPE_GLOBAL, size, __VA_ARGS__)
#define HID_PHYSICAL_MAXIMUM(size, ...) HID_REPORT_ITEM(0x4, HID_REPORT_ITEM_TYPE_GLOBAL, size, __VA_ARGS__)
#define HID_UNIT_EXPONENT(size, ...)    HID_REPORT_ITEM(0x5, HID_REPORT_ITEM_TYPE_GLOBAL, size, __VA_ARGS__)
#define HID_UNIT(size, ...)             HID_REPORT_ITEM(0x6, HID_REPORT_ITEM_TYPE_GLOBAL, size, __VA_ARGS__)
#define HID_REPORT_SIZE(size, ...)      HID_REPORT_ITEM(0x7, HID_REPORT_ITEM_TYPE_GLOBAL, size, __VA_ARGS__)
#define HID_REPORT_ID(size, ...)        HID_REPORT_ITEM(0x8, HID_REPORT_ITEM_TYPE_GLOBAL, size, __VA_ARGS__)
#define HID_REPORT_COUNT(size, ...)     HID_REPORT_ITEM(0x9, HID_REPORT_ITEM_TYPE_GLOBAL, size, __VA_ARGS__)
#define HID_PUSH(size, ...)             HID_REPORT_ITEM(0xA, HID_REPORT_ITEM_TYPE_GLOBAL, size, __VA_ARGS__)
#define HID_POP(size, ...)              HID_REPORT_ITEM(0xB, HID_REPORT_ITEM_TYPE_GLOBAL, size, __VA_ARGS__)

/* USB HID 1.11 Specification, section 6.2.2.8 Local Items, p. 39 */
#define HID_USAGE(size, ...)              HID_REPORT_ITEM(0x0, HID_REPORT_ITEM_TYPE_LOCAL, size, __VA_ARGS__)
#define HID_USAGE_MINIMUM(size, ...)      HID_REPORT_ITEM(0x1, HID_REPORT_ITEM_TYPE_LOCAL, size, __VA_ARGS__)
#define HID_USAGE_MAXIMUM(size, ...)      HID_REPORT_ITEM(0x2, HID_REPORT_ITEM_TYPE_LOCAL, size, __VA_ARGS__)
#define HID_DESIGNATOR_INDEX(size, ...)   HID_REPORT_ITEM(0x3, HID_REPORT_ITEM_TYPE_LOCAL, size, __VA_ARGS__)
#define HID_DESIGNATOR_MINIMUM(size, ...) HID_REPORT_ITEM(0x4, HID_REPORT_ITEM_TYPE_LOCAL, size, __VA_ARGS__)
#define HID_DESIGNATOR_MAXIMUM(size, ...) HID_REPORT_ITEM(0x5, HID_REPORT_ITEM_TYPE_LOCAL, size, __VA_ARGS__)
#define HID_STRING_INDEX(size, ...)       HID_REPORT_ITEM(0x7, HID_REPORT_ITEM_TYPE_LOCAL, size, __VA_ARGS__)
#define HID_STRING_MINIMUM(size, ...)     HID_REPORT_ITEM(0x8, HID_REPORT_ITEM_TYPE_LOCAL, size, __VA_ARGS__)
#define HID_STRING_MAXIMUM(size, ...)     HID_REPORT_ITEM(0x9, HID_REPORT_ITEM_TYPE_LOCAL, size, __VA_ARGS__)
#define HID_DELIMITER(size, ...)          HID_REPORT_ITEM(0xA, HID_REPORT_ITEM_TYPE_LOCAL, size, __VA_ARGS__)

/* USB HID 1.11 Specification, section 7.1 Standard Requests, p. 49 */
enum {
	HID_DescriptorType_HID                = 0x21,
	HID_DescriptorType_Report             = 0x22,
	HID_DescriptorType_PhysicalDescriptor = 0x23
} HID_DescriptorType_t;

/* USB HID 1.11 Specification, section 7.2 Class-Specific Requests, p. 50 */
enum {
	HID_RequestCode_GetReport   = 0x01,
	HID_RequestCode_GetIdle     = 0x02,
	HID_RequestCode_GetProtocol = 0x03,
	HID_RequestCode_SetReport   = 0x09,
	HID_RequestCode_SetIdle     = 0x0A,
	HID_RequestCode_SetProtocol = 0x0B
} HID_RequestCode_t;

/* USB HID Usage Tables 1.12, Section 3 Usage Pages, p. 14 */
#define HID_USAGE_PAGE_GENERIC_DESKTOP_CONTROLS 0x01
#define HID_USAGE_PAGE_KEYBOARD_KEYPAD          0x07
#define HID_USAGE_PAGE_LEDS                     0x08

/* USB HID Usage Tables 1.12, Section 4 Generic Desktop Page (0x01), p. 26 */
#define HID_DESKTOP_KEYBOARD 0x06

/* USB HID Usage Tables 1.12, Section 10 Keyboard/Keypad Page (0x07), p. 53 */
#define HID_KEYBOARD_ERROR_ROLLOVER        0x01
#define HID_KEYBOARD_POST_FAIL             0x02
#define HID_KEYBOARD_ERROR_UNDEFINED       0x03
#define HID_KEYBOARD_A                     0x04
#define HID_KEYBOARD_B                     0x05
#define HID_KEYBOARD_C                     0x06
#define HID_KEYBOARD_D                     0x07
#define HID_KEYBOARD_E                     0x08
#define HID_KEYBOARD_F                     0x09
#define HID_KEYBOARD_G                     0x0A
#define HID_KEYBOARD_H                     0x0B
#define HID_KEYBOARD_I                     0x0C
#define HID_KEYBOARD_J                     0x0D
#define HID_KEYBOARD_K                     0x0E
#define HID_KEYBOARD_L                     0x0F
#define HID_KEYBOARD_M                     0x10
#define HID_KEYBOARD_N                     0x11
#define HID_KEYBOARD_O                     0x12
#define HID_KEYBOARD_P                     0x13
#define HID_KEYBOARD_Q                     0x14
#define HID_KEYBOARD_R                     0x15
#define HID_KEYBOARD_S                     0x16
#define HID_KEYBOARD_T                     0x17
#define HID_KEYBOARD_U                     0x18
#define HID_KEYBOARD_V                     0x19
#define HID_KEYBOARD_W                     0x1A
#define HID_KEYBOARD_X                     0x1B
#define HID_KEYBOARD_Y                     0x1C
#define HID_KEYBOARD_Z                     0x1D
#define HID_KEYBOARD_1_BANG                0x1E
#define HID_KEYBOARD_2_AT                  0x1F
#define HID_KEYBOARD_3_HASH                0x20
#define HID_KEYBOARD_4_DOLLAR              0x21
#define HID_KEYBOARD_5_PERCENT             0x22
#define HID_KEYBOARD_6_CARET               0x23
#define HID_KEYBOARD_7_AMPERSAND           0x24
#define HID_KEYBOARD_8_ASTERISK            0x25
#define HID_KEYBOARD_9_LEFT_PAREN          0x26
#define HID_KEYBOARD_0_RIGHT_PAREN         0x27
#define HID_KEYBOARD_ENTER                 0x28
#define HID_KEYBOARD_ESCAPE                0x29
#define HID_KEYBOARD_BACKSPACE             0x2A
#define HID_KEYBOARD_TAB                   0x2B
#define HID_KEYBOARD_SPACEBAR              0x2C
#define HID_KEYBOARD_DASH_UNDERSCORE       0x2D
#define HID_KEYBOARD_EQUALS_PLUS           0x2E
#define HID_KEYBOARD_LBRACKET_LBRACE       0x2F
#define HID_KEYBOARD_RBRACKET_RBRACE       0x30
#define HID_KEYBOARD_BACKSLASH_PIPE        0x31
#define HID_KEYBOARD_NON_US_HASH_TILDE     0x32
#define HID_KEYBOARD_SEMICOLON_COLON       0x33
#define HID_KEYBOARD_APOSTROPHE_QUOTE      0x34
#define HID_KEYBOARD_BACKTICK_TILDE        0x35
#define HID_KEYBOARD_COMMA_LANGLE          0x36
#define HID_KEYBOARD_DOT_RANGLE            0x37
#define HID_KEYBOARD_SLASH_QUESTION        0x38
#define HID_KEYBOARD_CAPS_LOCK             0x39
#define HID_KEYBOARD_F1                    0x3A
#define HID_KEYBOARD_F2                    0x3B
#define HID_KEYBOARD_F3                    0x3C
#define HID_KEYBOARD_F4                    0x3D
#define HID_KEYBOARD_F5                    0x3E
#define HID_KEYBOARD_F6                    0x3F
#define HID_KEYBOARD_F7                    0x40
#define HID_KEYBOARD_F8                    0x41
#define HID_KEYBOARD_F9                    0x42
#define HID_KEYBOARD_F10                   0x43
#define HID_KEYBOARD_F11                   0x44
#define HID_KEYBOARD_F12                   0x45
#define HID_KEYBOARD_PRINT_SCREEN          0x46
#define HID_KEYBOARD_SCROLL_LOCK           0x47
#define HID_KEYBOARD_PAUSE                 0x48
#define HID_KEYBOARD_INSERT                0x49
#define HID_KEYBOARD_HOME                  0x4A
#define HID_KEYBOARD_PAGE_UP               0x4B
#define HID_KEYBOARD_DELETE                0x4C
#define HID_KEYBOARD_END                   0x4D
#define HID_KEYBOARD_PAGE_DOWN             0x4E
#define HID_KEYBOARD_RIGHT_ARROW           0x4F
#define HID_KEYBOARD_LEFT_ARROW            0x50
#define HID_KEYBOARD_DOWN_ARROW            0x51
#define HID_KEYBOARD_UP_ARROW              0x52
#define HID_KEYPAD_NUM_LOCK                0x53
#define HID_KEYPAD_SLASH                   0x54
#define HID_KEYPAD_ASTERISK                0x55
#define HID_KEYPAD_DASH                    0x56
#define HID_KEYPAD_PLUS                    0x57
#define HID_KEYPAD_ENTER                   0x58
#define HID_KEYPAD_1_END                   0x59
#define HID_KEYPAD_2_DOWN_ARROW            0x5A
#define HID_KEYPAD_3_PAGE_DOWN             0x5B
#define HID_KEYPAD_4_LEFT_ARROW            0x5C
#define HID_KEYPAD_5                       0x5D
#define HID_KEYPAD_6_RIGHT_ARROW           0x5E
#define HID_KEYPAD_7_HOME                  0x5F
#define HID_KEYPAD_8_UP_ARROW              0x60
#define HID_KEYPAD_9_PAGE_UP               0x61
#define HID_KEYPAD_0_INSERT                0x62
#define HID_KEYPAD_DOT_DELETE              0x63
#define HID_KEYBOARD_NON_US_BACKSLASH_PIPE 0x64
#define HID_KEYBOARD_APPLICATION           0x65
#define HID_KEYBOARD_POWER                 0x66
#define HID_KEYPAD_EQUALS                  0x67
#define HID_KEYBOARD_F13                   0x68
#define HID_KEYBOARD_F14                   0x69
#define HID_KEYBOARD_F15                   0x6A
#define HID_KEYBOARD_F16                   0x6B
#define HID_KEYBOARD_F17                   0x6C
#define HID_KEYBOARD_F18                   0x6D
#define HID_KEYBOARD_F19                   0x6E
#define HID_KEYBOARD_F20                   0x6F
#define HID_KEYBOARD_F21                   0x70
#define HID_KEYBOARD_F22                   0x71
#define HID_KEYBOARD_F23                   0x72
#define HID_KEYBOARD_F24                   0x73
#define HID_KEYBOARD_EXECUTE               0x74
#define HID_KEYBOARD_HELP                  0x75
#define HID_KEYBOARD_MENU                  0x76
#define HID_KEYBOARD_SELECT                0x77
#define HID_KEYBOARD_STOP                  0x78
#define HID_KEYBOARD_AGAIN                 0x79
#define HID_KEYBOARD_UNDO                  0x7A
#define HID_KEYBOARD_CUT                   0x7B
#define HID_KEYBOARD_COPY                  0x7C
#define HID_KEYBOARD_PASTE                 0x7D
#define HID_KEYBOARD_FIND                  0x7E
#define HID_KEYBOARD_MUTE                  0x7F
#define HID_KEYBOARD_VOLUME_UP             0x80
#define HID_KEYBOARD_VOLUME_DOWN           0x81
#define HID_KEYBOARD_LOCKING_CAPS_LOCK     0x82
#define HID_KEYBOARD_LOCKING_NUM_LOCK      0x83
#define HID_KEYBOARD_LOCKING_SCROLL_LOCK   0x84
#define HID_KEYPAD_COMMA                   0x85
#define HID_KEYPAD_EQUALS_AS400            0x86
#define HID_KEYBOARD_INTERNATIONAL1        0x87
#define HID_KEYBOARD_INTERNATIONAL2        0x88
#define HID_KEYBOARD_INTERNATIONAL3        0x89
#define HID_KEYBOARD_INTERNATIONAL4        0x8A
#define HID_KEYBOARD_INTERNATIONAL5        0x8B
#define HID_KEYBOARD_INTERNATIONAL6        0x8C
#define HID_KEYBOARD_INTERNATIONAL7        0x8D
#define HID_KEYBOARD_INTERNATIONAL8        0x8E
#define HID_KEYBOARD_INTERNATIONAL9        0x8F
#define HID_KEYBOARD_LANG1                 0x90
#define HID_KEYBOARD_LANG2                 0x91
#define HID_KEYBOARD_LANG3                 0x92
#define HID_KEYBOARD_LANG4                 0x93
#define HID_KEYBOARD_LANG5                 0x94
#define HID_KEYBOARD_LANG6                 0x95
#define HID_KEYBOARD_LANG7                 0x96
#define HID_KEYBOARD_LANG8                 0x97
#define HID_KEYBOARD_LANG9                 0x98
#define HID_KEYBOARD_ALTERNATE_ERASE       0x99
#define HID_KEYBOARD_SYSREQ_ATTENTION      0x9A
#define HID_KEYBOARD_CANCEL                0x9B
#define HID_KEYBOARD_CLEAR                 0x9C
#define HID_KEYBOARD_PRIOR                 0x9D
#define HID_KEYBOARD_RETURN                0x9E
#define HID_KEYBOARD_SEPARATOR             0x9F
#define HID_KEYBOARD_OUT                   0xA0
#define HID_KEYBOARD_OPER                  0xA1
#define HID_KEYBOARD_CLEAR_AGAIN           0xA2
#define HID_KEYBOARD_CRSEL_PROPS           0xA3
#define HID_KEYBOARD_EXSEL                 0xA4
#define HID_KEYPAD_00                      0xB0
#define HID_KEYPAD_000                     0xB1
#define HID_KEYPAD_THOUSANDS_SEPARATOR     0xB2
#define HID_KEYPAD_DECIMAL_SEPARATOR       0xB3
#define HID_KEYPAD_CURRENCY_UNIT           0xB4
#define HID_KEYPAD_CURRENCY_SUB_UNIT       0xB5
#define HID_KEYPAD_LPAREN                  0xB6
#define HID_KEYPAD_RPAREN                  0xB7
#define HID_KEYPAD_LBRACE                  0xB8
#define HID_KEYPAD_RBRACE                  0xB9
#define HID_KEYPAD_TAB                     0xBA
#define HID_KEYPAD_BACKSPACE               0xBB
#define HID_KEYPAD_A                       0xBC
#define HID_KEYPAD_B                       0xBD
#define HID_KEYPAD_C                       0xBE
#define HID_KEYPAD_D                       0xBF
#define HID_KEYPAD_E                       0xC0
#define HID_KEYPAD_F                       0xC1
#define HID_KEYPAD_XOR                     0xC2
#define HID_KEYPAD_CARET                   0xC3
#define HID_KEYPAD_PERCENT                 0xC4
#define HID_KEYPAD_LANGLE                  0xC5
#define HID_KEYPAD_RANGLE                  0xC6
#define HID_KEYPAD_AMPERSAND               0xC7
#define HID_KEYPAD_DOUBLE_AMPERSAND        0xC8
#define HID_KEYPAD_PIPE                    0xC9
#define HID_KEYPAD_DOUBLE_PIPE             0xCA
#define HID_KEYPAD_COLON                   0xCB
#define HID_KEYPAD_HASH                    0xCC
#define HID_KEYPAD_SPACE                   0xCD
#define HID_KEYPAD_AT                      0xCE
#define HID_KEYPAD_BANG                    0xCF
#define HID_KEYPAD_MEMORY_STORE            0xD0
#define HID_KEYPAD_MEMORY_RECALL           0xD1
#define HID_KEYPAD_MEMORY_CLEAR            0xD2
#define HID_KEYPAD_MEMORY_ADD              0xD3
#define HID_KEYPAD_MEMORY_SUBTRACT         0xD4
#define HID_KEYPAD_MEMORY_MULTIPLY         0xD5
#define HID_KEYPAD_MEMORY_DIVIDE           0xD6
#define HID_KEYPAD_PLUS_MINUS              0xD7
#define HID_KEYPAD_CLEAR                   0xD8
#define HID_KEYPAD_CLEAR_ENTRY             0xD9
#define HID_KEYPAD_BINARY                  0xDA
#define HID_KEYPAD_OCTAL                   0xDB
#define HID_KEYPAD_DECIMAL                 0xDC
#define HID_KEYPAD_HEXADECIMAL             0xDD
#define HID_KEYBOARD_LEFT_CONTROL          0xE0
#define HID_KEYBOARD_LEFT_SHIFT            0xE1
#define HID_KEYBOARD_LEFT_ALT              0xE2
#define HID_KEYBOARD_LEFT_GUI              0xE3
#define HID_KEYBOARD_RIGHT_CONTROL         0xE4
#define HID_KEYBOARD_RIGHT_SHIFT           0xE5
#define HID_KEYBOARD_RIGHT_ALT             0xE6
#define HID_KEYBOARD_RIGHT_GUI             0xE7

/* USB HID Usage Tables 1.12, Section 11 LED Page (0x08), p. 61 */
#define HID_LED_NUM_LOCK    0x01
#define HID_LED_CAPS_LOCK   0x02
#define HID_LED_SCROLL_LOCK 0x03
#define HID_LED_COMPOSE     0x04
#define HID_LED_KANA        0x05
