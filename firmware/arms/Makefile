TARGET=arms
MCU=attiny88
CPU=8000000

include ../shared/shared.mk

fuse:
	$(AVRDUDE) -U lfuse:w:0x6E:m -U hfuse:w:0xDF:m -U efuse:w:0xFF:m

.PHONY: fuse
