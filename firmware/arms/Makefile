TARGET=arms
MCU=attiny88
CPU=8000000

include ../shared/shared.mk

fuse:
	avrdude -v -p $(MCU) -c arduino -P ${PORT} -U lfuse:w:0xEE:m

.PHONY: fuse
