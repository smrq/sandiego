#include "defs.h"
#include "./twi.h"
#include "buffers/leds.h"
#include "buffers/keys.h"

enum MessageState_t {
	IDLE,
	SET_LEDS_COMMAND,
	SET_LEDS_COLOR,
	GET_KEY_STATE_COMMAND,
	GET_KEY_STATE_DATA,
	FINISHED
};

local u8 messageRecipient = 0;
local void *messageBuffer;
local volatile u16 messageIndex = 0;
local volatile enum MessageState_t messageState = IDLE;

void TWI_init() {
	/*
		TWBR should be 10 or higher if the TWI operates in Master mode. If TWBR is lower than 10,
		the Master may produce an incorrect output on SDA and SCL for the reminder of the byte.
		The problem occurs when operating the TWI in Master mode, sending Start + SLA + R/W to a
		Slave (a Slave does not need to be connected to the bus for the condition to happen).
			- ATmega32U4 datasheet, p. 231
	*/
	TWBR = 10;

	// Set bit rate prescaler to 1
	TWSR &= ~(_BV(TWPS0) | _BV(TWPS1));
}

local void TWI_start() {
	TWCR = _BV(TWEN) | _BV(TWIE) | _BV(TWINT) |  _BV(TWSTA);
}

local void TWI_continue(bool ack) {
	TWCR = _BV(TWEN) | _BV(TWIE) | _BV(TWINT) | (ack ? _BV(TWEA) : 0);
}

local void TWI_stop() {
	TWCR = _BV(TWEN) | _BV(TWIE) | _BV(TWINT) | _BV(TWSTO);
	messageState = IDLE;
}

bool TWI_busy() {
	return messageState != IDLE || (TWCR & _BV(TWSTO));
}

bool TWI_setLeds(u8 address, led_buffer_t *buffer) {
	if (TWI_busy()) {
		return false;
	}

	messageRecipient = address;
	messageBuffer = (void *)buffer;
	messageIndex = 0;
	messageState = SET_LEDS_COMMAND;
	TWI_start();

	return true;
}

bool TWI_getKeyState(u8 address, key_buffer_t *buffer) {
	if (TWI_busy()) {
		return false;
	}

	messageRecipient = address;
	messageBuffer = (void *)buffer;
	messageIndex = 0;
	messageState = GET_KEY_STATE_COMMAND;
	TWI_start();

	return true;
}

ISR(TWI_vect) {
	switch (TW_STATUS) {
		case TW_START: // 0x08
		case TW_REP_START: // 0x10
			switch (messageState) {
				case SET_LEDS_COMMAND:
				case GET_KEY_STATE_COMMAND:
					TWDR = (messageRecipient << 1) | TW_WRITE;
					TWI_continue(true); // SLA+W will be transmitted; ACK or NOT ACK will be received
					break;

				case GET_KEY_STATE_DATA:
					TWDR = (messageRecipient << 1) | TW_READ;
					TWI_continue(true); // SLA+R will be transmitted; ACK or NOT ACK will be received
					break;

				default: // This should never happen.
					TWDR = 0xFE | TW_WRITE; // Nobody should respond to this.
					TWI_continue(true); // SLA+W will be transmitted; ACK or NOT ACK will be received
					break;
			}
			break;

		// Master transmit mode
		case TW_MT_SLA_ACK: // 0x18
		case TW_MT_DATA_ACK: // 0x28
			switch (messageState) {
				case SET_LEDS_COMMAND:
					TWDR = TWI_CMD_SET_LEDS;
					messageState = SET_LEDS_COLOR;
					TWI_continue(true); // Data byte will be transmitted and ACK or NOT ACK will be received
					break;

				case SET_LEDS_COLOR: {
					led_buffer_t *ledBuffer = (led_buffer_t *)messageBuffer;
					TWDR = ((u8 *)(ledBuffer->front))[messageIndex];
					if (++messageIndex == LED_COUNT * sizeof(ledBuffer->front[0])) {
						messageState = FINISHED;
					}
					TWI_continue(true); // Data byte will be transmitted and ACK or NOT ACK will be received
					break;
				}

				case GET_KEY_STATE_COMMAND:
					TWDR = TWI_CMD_GET_KEY_STATE;
					messageState = GET_KEY_STATE_DATA;
					TWI_continue(true); // Data byte will be transmitted and ACK or NOT ACK will be received
					break;

				case GET_KEY_STATE_DATA:
					TWI_start(); // Repeated START will be transmitted
					break;

				case FINISHED:
					TWI_stop(); // STOP condition will be transmitted and TWSTO Flag will be reset
					break;

				default: // This should never happen.
					TWI_stop(); // STOP condition will be transmitted and TWSTO Flag will be reset
					break;
			}
			break;

		// Master receive mode
		case TW_MR_SLA_ACK: // 0x40
			TWI_continue(true); // Data byte will be received and ACK will be returned
			break;

		case TW_MR_DATA_ACK: // 0x50
		case TW_MR_DATA_NACK: // 0x58
			switch (messageState) {
				case GET_KEY_STATE_DATA: {
					key_buffer_t *keyBuffer = (key_buffer_t *)messageBuffer;
					keyBuffer->back[messageIndex] = TWDR;

					if (TW_STATUS == TW_MR_DATA_NACK) {
						TWI_stop(); // STOP condition will be transmitted and TWSTO Flag will be reset
					} else if (++messageIndex == ROW_COUNT - 1) {
						TWI_continue(false); // Data byte will be received and NOT ACK will be returned
					} else {
						TWI_continue(true); // Data byte will be received and ACK will be returned
					}
					break;
				}

				default: // This should never happen.
					TWI_stop(); // STOP condition will be transmitted and TWSTO Flag will be reset
					break;
			}
			break;

		// Error cases
		case TW_MT_SLA_NACK: // 0x20
		case TW_MT_DATA_NACK: // 0x28
		case TW_MR_SLA_NACK: // 0x48
			TWI_stop(); // STOP condition will be transmitted and TWSTO Flag will be reset
			break;

		case TW_MT_ARB_LOST: // and TW_MR_ARB_LOST: 0x38  This should never happen because we are the only master
			TWI_start(); // A START condition will be transmitted when the bus becomes free
			break;

		case TW_BUS_ERROR: // 0x00
		default:
			// Only the internal hardware is affected, no STOP condition is sent on the bus.
			// In all cases, the bus is released and TWSTO is cleared.
			TWI_stop();
			break;
	}
}
