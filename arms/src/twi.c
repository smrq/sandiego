#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/twi.h>
#include "defs.h"
#include "twi.h"

#include "heartbeat.h"

void (*TWI_requestedData)(u8 *dest, u8 *length);
void (*TWI_receivedData)(u8 *src, u8 length);

local u8 messageBuffer[TWI_BUFFER_SIZE];
local u8 messageSize = 0;

local void TWI_listen(bool ack) {
	TWCR = _BV(TWEN) | _BV(TWIE) | _BV(TWINT) | (ack ? _BV(TWEA) : 0);
}

local void TWI_recover() {
	/*
		In Slave mode, setting the TWSTO bit can be used to recover from an error condition.
		This will not generate a STOP condition, but the TWI returns to a well-defined unaddressed
		Slave mode and releases the SCL and SDA lines to a high impedance state.
			- ATtiny88 datasheet, p. 158
	*/
	TWCR = _BV(TWEN) | _BV(TWIE) | _BV(TWINT) | _BV(TWEA) | _BV(TWSTO);
	while (TWCR & _BV(TWSTO));
}

void TWI_init(u8 address, bool acceptGeneral) {
	/*
		The upper 7 bits are the address to which the 2-wire Serial Interface will respond when
		addressed by a Master. If the LSB is set, the TWI will respond to the general call address (0x00),
		otherwise it will ignore the general call address.
			- ATtiny88 datasheet, p. 148
	*/
	TWAR = address << 1 | acceptGeneral;

	// TODO: Pretty sure this isn't necessary.
	TWDR = 0xFF;

	// Set bit rate prescaler to 1
	TWSR &= ~(_BV(TWPS0) | _BV(TWPS1));

	TWI_listen(true);
}

ISR(TWI_vect) {
	static u8 bufferIndex;

	switch (TWSR) {
		case TW_NO_INFO:
			break;

		// Slave receive mode
		case TW_SR_SLA_ACK:
		case TW_SR_GCALL_ACK:
		case TW_SR_ARB_LOST_SLA_ACK: // This should never happen because we are never a master
		case TW_SR_ARB_LOST_GCALL_ACK: // This should never happen because we are never a master
			beat();
			bufferIndex = 0;
			TWI_listen(true);
			break;

		case TW_SR_DATA_ACK:
		case TW_SR_GCALL_DATA_ACK:
			if (bufferIndex < TWI_BUFFER_SIZE) {
				messageBuffer[bufferIndex++] = TWDR;
				TWI_listen(true);
			} else {
				TWI_listen(false);
			}
			break;

		case TW_SR_STOP:
			TWI_recover(); // TODO: Pretty sure this isn't necessary.
			if (TWI_receivedData) {
				TWI_receivedData(messageBuffer, bufferIndex);
			}
			TWI_listen(true);
			break;

		// Slave transmit mode
		case TW_ST_SLA_ACK:
		case TW_ST_ARB_LOST_SLA_ACK: // This should never happen because we are never a master
			bufferIndex = 0;

			if (TWI_requestedData) {
				messageSize = TWI_BUFFER_SIZE;
				TWI_requestedData(messageBuffer, &messageSize);
			} else {
				messageSize = 0;
			}

			// Empty messages are not valid
			if (messageSize == 0) {
				messageSize = 1;
				messageBuffer[0] = 0;
			}

			/* fall through */

		case TW_ST_DATA_ACK:
			TWDR = messageBuffer[bufferIndex++];
			TWI_listen(bufferIndex < messageSize);
			break;

		// Error cases
		case TW_ST_DATA_NACK:
		case TW_ST_LAST_DATA:
		case TW_SR_DATA_NACK:
		case TW_SR_GCALL_DATA_NACK:
			/*
				Switched to the not addressed Slave mode; own SLA will be recognized;
					- ATtiny datasheet, p. 149 (SR) & 152 (ST)
			*/
			TWI_listen(true);
			break;

		case TW_BUS_ERROR: // Bus error due to an illegal START or STOP condition
		default:
			TWI_recover();
			break;
	}
}
