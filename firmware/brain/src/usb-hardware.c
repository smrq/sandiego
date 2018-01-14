#include "defs.h"
#include "usb.h"

void USB_enablePLL() {
	/*
		When the PLOCK bit is set, the PLL is locked to the reference clock. After the PLL
		is enabled, it takes about several ms for the PLL to lock.
			- ATmega32U4 datasheet, p. 41
	*/
	PLLCSR = USB_PLL_PRESCALE;
	PLLCSR = USB_PLL_PRESCALE | _BV(PLLE);
	while (!(PLLCSR & _BV(PLOCK)));
}

void USB_disablePLL() {
	PLLCSR = 0;
}

void USB_setAddress(u8 address) {
	/*
		Bit 7 - ADDEN - Address Enable Bit
			(...) Cleared by hardware. Clearing by software has no effect.
		Bits 6-0 - UADD6:0 - USB Address Bits
			Load by software to configure the device address
			- ATmega32U4 datasheet, p. 284
	*/
	UDADDR = address & 0x7F;
}

void USB_enableAddress() {
	/*
		Bit 7 - ADDEN - Address Enable Bit
			Set to activate the UADD (USB address).
			- ATmega32U4 datasheet, p. 284
	*/
	UDADDR |= _BV(ADDEN);
}

/* ATmega32U4 datasheet, section 22.6 Endpoint Activation, p. 271 */
bool USB_configureEndpoint(u8 number, u8 flags0, u8 flags1) {
	UENUM = number;
	UECONX = _BV(EPEN);
	UECFG0X = flags0;
	UECFG1X = flags1 | _BV(ALLOC);
	return UESTA0X & _BV(CFGOK);
}

void USB_resetEndpoint(u8 number) {
	/*
		Bits 6-0 - EPRST6:0 - Endpoint FIFO Reset Bits
		Set to reset the selected endpoint FIFO prior to any other operation, upon hardware reset or when an USB bus
		reset has been received. See “Endpoint Reset” on page 270 for more information
		Then, clear by software to complete the reset operation and start using the endpoint.
			- ATmega32U4 datasheet, p. 285
	*/
	UERST = _BV(number);
	UERST = 0;
}

void USB_resetEndpointDataToggle() {
	UECONX |= _BV(RSTDT);
}

bool USB_isEndpointEnabled() {
	return UECONX & _BV(EPEN);
}

void USB_selectEndpoint(u8 number) {
	UENUM = number;
}

u8 USB_getSelectedEndpoint() {
	return UENUM;
}

u8 USB_readByteFromEndpoint() {
	return UEDATX;
}

void USB_readFromEndpoint(void *buffer, u8 length) {
	for (u8 *ptr = (u8 *)buffer; ptr < (u8 *)buffer + length; ++ptr) {
		*ptr = UEDATX;
	}
}

void USB_writeByteToEndpoint(u8 byte) {
	UEDATX = byte;
}

u16 USB_bytesInEndpoint() {
	return ((u16)UEBCHX << 8) | UEBCLX;
}

void USB_stallEndpoint() {
	UECONX |= _BV(STALLRQ);
}

void USB_clearEndpointStall() {
	UECONX |= _BV(STALLRQC);
}

bool USB_isEndpointStalled() {
	return UECONX & _BV(STALLRQ);
}

bool USB_isINReady() {
	return UEINTX & _BV(TXINI);
}

bool USB_waitForINReady() {
	while (!USB_isINReady()) {
		if (USB_DEVICE_STATE == USB_DeviceState_Detached ||
			USB_DEVICE_STATE == USB_DeviceState_Suspended ||
			USB_isEndpointStalled()) {
			return false;
		}
	}
	return true;
}

void USB_sendIN() {
	UEINTX &= ~(_BV(TXINI) | _BV(FIFOCON));
}

bool USB_isOUTReady() {
	return UEINTX & _BV(RXOUTI);
}

bool USB_waitForOUTReady() {
	while (!USB_isOUTReady()) {
		if (USB_DEVICE_STATE == USB_DeviceState_Detached ||
			USB_DEVICE_STATE == USB_DeviceState_Suspended ||
			USB_isEndpointStalled()) {
			return false;
		}
	}
	return true;
}

void USB_clearOUT() {
	UEINTX &= ~(_BV(RXOUTI) | _BV(FIFOCON));
}

bool USB_isSETUPReady() {
	return UEINTX & _BV(RXSTPI);
}

void USB_clearSETUP() {
	UEINTX &= ~_BV(RXSTPI);
}

bool USB_transferData(const void *buffer, u8 length, bool fromPROGMEM, u8 endpointSize) {
	do {
		while (!USB_isINReady()) {
			if (USB_DEVICE_STATE == USB_DeviceState_Detached ||
				USB_DEVICE_STATE == USB_DeviceState_Suspended ||
				USB_isEndpointStalled() ||
				USB_isSETUPReady()) {
				return false;
			}
			if (USB_isOUTReady()) {
				break;
			}
		}

		if (length) {
			u16 remainingSpace = endpointSize - USB_bytesInEndpoint();
			while (length && remainingSpace) {
				USB_writeByteToEndpoint(fromPROGMEM ? pgm_read_byte((u8 *)buffer++) : *(u8 *)buffer++);
				length--;
				remainingSpace--;
			}
		}

		USB_sendIN();
	} while (length);

	return true;
}
