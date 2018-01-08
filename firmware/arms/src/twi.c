#include "defs.h"
#include "debug.h"
#include "leds.h"
#include "keyscanner.h"
#include "spi.h"
#include "twi.h"

local u8 messageBuffer[TWI_BUFFER_SIZE] = { 0 };
local u8 messageSize = 0;
local u8 messageIndex = 0;
local enum MessageState {
	IDLE,
	COMMAND,
	SET_LEDS,
	GET_KEY_STATE
} messageState = IDLE;

local void TWI_continue(bool ack) {
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

	TWI_continue(true);
}

ISR(TWI_vect) {
	switch (TW_STATUS) {
		// Slave receive mode
		case TW_SR_SLA_ACK: // 0x60
		case TW_SR_GCALL_ACK: // 0x70
		case TW_SR_ARB_LOST_SLA_ACK: // 0x68  This should never happen because we are never a master
		case TW_SR_ARB_LOST_GCALL_ACK: // 0x78  This should never happen because we are never a master
			messageState = COMMAND;
			TWI_continue(true); // Data byte will be received and ACK will be returned
			break;

		case TW_SR_DATA_ACK: // 0x80
		case TW_SR_GCALL_DATA_ACK: // 0x90
			switch (messageState) {
				case COMMAND:
					switch (TWDR) {
						case TWI_CMD_SET_LEDS:
							messageSize = 0;
							messageIndex = 0;
							messageState = SET_LEDS;
							break;

						case TWI_CMD_GET_KEY_STATE:
							messageState = GET_KEY_STATE;
							break;

						default: // This should never happen.
							messageState = IDLE;
							break;
					}
					TWI_continue(true); // Data byte will be received and ACK will be returned
					break;

				case SET_LEDS:
					messageBuffer[messageSize++] = TWDR;
					if (messageSize == 4) {
						u32 color = *(u32 *)(messageBuffer);
						led_setColor(messageIndex, color);
						messageSize = 0;

						if (++messageIndex == LED_COUNT) {
							SPI_transmit();
							messageState = IDLE;
						}
					}
					TWI_continue(true); // Data byte will be received and ACK will be returned
					break;

				default: // This should never happen.
					TWI_continue(false); // Data byte will be received and NOT ACK will be returned
					break;
			}
			break;

		case TW_SR_STOP: // 0xA0
			// TODO: This is commented out because we're getting a STOP in the middle of
			// the GET_KEY_STATE command. Why isn't it a repeat start?
			// messageState = IDLE;
			TWI_continue(true); // Switched to the not addressed Slave mode; own SLA will be recognized
			break;

		// Slave transmit mode
		case TW_ST_SLA_ACK: // 0xA8
		case TW_ST_ARB_LOST_SLA_ACK: // 0xB0  This should never happen because we are never a master
			messageIndex = 0;
			/* fall through */

		case TW_ST_DATA_ACK: // 0xB8
			switch (messageState) {
				case GET_KEY_STATE: {
					u8 rowState = keyscanner_getRowState(messageIndex);
					TWDR = rowState;
					if (++messageIndex == ROW_COUNT) {
						TWI_continue(false); // Last data byte will be transmitted and NOT ACK should be received
					} else {
						TWI_continue(true); // Data byte will be transmitted and ACK should be received
					}
					break;
				}

				default: // This should never happen.
					TWI_continue(false); // Last data byte will be transmitted and NOT ACK should be received
					break;
			}
			break;

		case TW_ST_DATA_NACK: // 0xC0
			messageState = IDLE;
			TWI_continue(true); // Switched to the not addressed Slave mode; own SLA will be recognized
			break;

		// Error cases
		case TW_ST_LAST_DATA: // 0xC8
		case TW_SR_DATA_NACK: // 0x88
		case TW_SR_GCALL_DATA_NACK: // 0x98
			TWI_continue(true); // Switched to the not addressed Slave mode; own SLA will be recognized
			break;

		case TW_NO_INFO: // 0xF8
			break;

		case TW_BUS_ERROR: // 0x00
		default:
			// Only the internal hardware is affected, no STOP condition is sent on the bus.
			// In all cases, the bus is released and TWSTO is cleared.
			TWI_recover();
			break;
	}
}
