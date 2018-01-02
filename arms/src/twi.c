#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/twi.h>
#include "defs.h"
#include "led.h"
#include "twi.h"

local u8 messageBuffer[TWI_BUFFER_SIZE];
local u8 messageSize = 0;
local u8 messageIndex;
local enum MessageState {
	IDLE,
	COMMAND,
	SET_LED,
	SET_ALL_LEDS,
	SET_LED_BANK
} messageState = IDLE;

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

	// Set bit rate prescaler to 1
	TWSR &= ~(_BV(TWPS0) | _BV(TWPS1));

	TWI_listen(true);
}

ISR(TWI_vect) {
	switch (TW_STATUS) {
		// Slave receive mode
		case TW_SR_SLA_ACK: // 0x60
		case TW_SR_GCALL_ACK: // 0x70
		case TW_SR_ARB_LOST_SLA_ACK: // 0x68  This should never happen because we are never a master
		case TW_SR_ARB_LOST_GCALL_ACK: // 0x78  This should never happen because we are never a master
			messageState = COMMAND;
			TWI_listen(true);
			break;

		case TW_SR_DATA_ACK: // 0x80
		case TW_SR_GCALL_DATA_ACK: // 0x90
			switch (messageState) {
				case IDLE: // This should never happen.
					break;

				case COMMAND:
					switch (TWDR) {
						case TWI_CMD_SET_LED:
							messageSize = 0;
							messageState = SET_LED;
							break;

						case TWI_CMD_SET_ALL_LEDS:
							messageSize = 0;
							messageState = SET_ALL_LEDS;
							break;

						case TWI_CMD_SET_LED_BANK:
							messageSize = 0;
							messageIndex = 0;
							messageState = SET_LED_BANK;
							break;

						default:
							messageState = IDLE;
							break;
					}
					break;

				case SET_LED:
					messageBuffer[messageSize++] = TWDR;
					if (messageSize == 5) {
						u8 index = messageBuffer[0];
						u32 color = *(u32 *)(messageBuffer + 1);
						setLed(index, color);
						transmitLeds();
						messageState = IDLE;
					}
					break;

				case SET_ALL_LEDS:
					messageBuffer[messageSize++] = TWDR;
					if (messageSize == 4) {
						u32 color = *(u32 *)(messageBuffer);
						setAllLeds(color);
						transmitLeds();
						messageState = IDLE;
					}
					break;

				case SET_LED_BANK:
					messageBuffer[messageSize++] = TWDR;
					if (messageSize == 6) {
						bool flush = messageBuffer[0];
						u8 bank = messageBuffer[1];
						u8 index = (bank * LED_BANK_SIZE) + messageIndex++;
						u32 color = *(u32 *)(messageBuffer + 2);
						setLed(index, color);

						// Start accumulating another u32 color
						messageSize = 2;

						if (
							(messageIndex == LED_BANK_SIZE) ||
							(bank * LED_BANK_SIZE + messageIndex == LED_COUNT)
						) {
							if (flush) {
								transmitLeds();
							}
							messageState = IDLE;
						}
					}
					break;
			}
			TWI_listen(true);
			break;

		case TW_SR_STOP: // 0xA0
			messageState = IDLE;
			TWI_listen(true);
			break;

		// Slave transmit mode
		case TW_ST_SLA_ACK: // 0xA8
		case TW_ST_ARB_LOST_SLA_ACK: // 0xB0  This should never happen because we are never a master
			messageIndex = 0;
			messageSize = 0;
			TWI_requestedData(messageBuffer, &messageSize);

			// Empty messages are not valid
			if (messageSize == 0) {
				messageSize = 1;
				messageBuffer[0] = 0;
			}

			/* fall through */

		case TW_ST_DATA_ACK: // 0xB8
			TWDR = messageBuffer[messageIndex++];
			TWI_listen(messageIndex < messageSize);
			break;

		// Error cases
		case TW_NO_INFO: // 0xF8
			break;

		case TW_ST_DATA_NACK: // 0xC0
		case TW_ST_LAST_DATA: // 0xC8
		case TW_SR_DATA_NACK: // 0x88
		case TW_SR_GCALL_DATA_NACK: // 0x98
			/*
				This will reset the state of the TWI:

				Switched to the not addressed Slave mode; own SLA will be recognized;
					- ATtiny datasheet, p. 149 (SR) & 152 (ST)
			*/
			TWI_listen(true);
			break;

		case TW_BUS_ERROR: // 0x00
		default:
			TWI_recover();
			break;
	}
}
