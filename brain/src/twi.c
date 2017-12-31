#include <avr/io.h>
#include <avr/interrupt.h>
#include <string.h>
#include <util/twi.h>
#include "defs.h"
#include "twi.h"

local u8 messageBuffer[TWI_BUFFER_SIZE];
local u8 messageSize = 0;
local u8 messageRecipient = 0;
local u8 messageIndex = 0;

local volatile bool busy = false;

void TWI_init() {
	// Set bit rate prescaler to 1
	TWSR &= ~(_BV(TWPS0) | _BV(TWPS1));
}

local void TWI_start() {
	TWCR = _BV(TWEN) | _BV(TWIE) | _BV(TWINT) |  _BV(TWSTA);
}

local void TWI_continue() {
	TWCR = _BV(TWEN) | _BV(TWIE) | _BV(TWINT);
}

local void TWI_stop() {
	TWCR = _BV(TWEN) | _BV(TWIE) | _BV(TWINT) | _BV(TWSTO);
	busy = false;
}

bool TWI_busy() {
	return busy;
}

void TWI_transmit(u8 address, u8 *buffer, u8 length) {
	busy = true;
	memcpy(messageBuffer, buffer, length);
	messageSize = length;
	messageRecipient = address;
	TWI_start();
}

ISR(TWI_vect) {
	switch (TW_STATUS) {
		case TW_START: // 0x08
		case TW_REP_START: // 0x10  This should never happen because we do not use repeat starts
			messageIndex = 0;
			TWDR = (messageRecipient << 1) | TW_WRITE;
			TWI_continue();
			break;

		// Master transmit mode
		case TW_MT_SLA_ACK: // 0x18
		case TW_MT_DATA_ACK: // 0x28
			if (messageIndex < messageSize) {
				TWDR = messageBuffer[messageIndex++];
				TWI_continue();
			} else {
				TWI_stop();
			}
			break;

		case TW_MT_SLA_NACK:
		case TW_MT_DATA_NACK:
			// Not sure what we should do here
			TWI_stop();
			break;

		case TW_MT_ARB_LOST: // 0x38  This should never happen because we are the only master
		// case TW_MR_ARB_LOST: // Also 0x38  This should never happen because we are the only master
			/*
				A START condition will be transmitted when the bus becomes free
					- ATmega32u4 datasheet, p. 143
			*/
			TWI_start();
			break;

		case TW_BUS_ERROR: // 0x00
		default:
			break;
	}
}
