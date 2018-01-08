TARGET=brain
MCU=atmega32u4
CPU=16000000

include ../shared/shared.mk

fuse:
	$(AVRDUDE) -U lfuse:w:0x5F:m -U hfuse:w:0xDF:m -U efuse:w:0xF4:m

.PHONY: fuse
