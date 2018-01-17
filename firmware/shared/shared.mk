CC=avr-gcc

SRCDIR=src
OBJDIR=obj
BINDIR=bin

SHAREDDIR=$(abspath $(dir $(lastword $(MAKEFILE_LIST))))
VENDORDIR=$(abspath $(SHAREDDIR)/../vendor)

# NOTE: For whatever reason, the Teensy ArduinoISP seems to change tty every
# time it's plugged in.  On my system, it seems to always be the second
# tty.usbmodem* entry, so I just figure out what that is dynamically.
# If there's a way of determining this more robustly, that would be great.
PORT:=$(shell ls /dev/tty.usbmodem* | sed -n '2 p')

CFLAGS+=-Wall -Wextra -Werror=implicit-function-declaration -std=gnu11 -I$(VENDORDIR) -I$(SHAREDDIR) -I$(SRCDIR)
OPTFLAGS+=-O2
EXTOPTFLAGS+=-flto
ARCHFLAGS+=-mmcu=$(MCU) -DF_CPU=$(CPU)
AVRDUDEFLAGS+=-C $(SHAREDDIR)/avrdude.conf -p $(MCU) -c arduino -P $(PORT) -v

COMPILE=$(CC) $(CFLAGS) $(OPTFLAGS) $(EXTOPTFLAGS) $(ARCHFLAGS) -c
COMPILEASM=$(CC) $(CFLAGS) $(OPTFLAGS) $(ARCHFLAGS) -S
LINK=$(CC) $(OPTFLAGS) $(EXTOPTFLAGS) $(ARCHFLAGS)
AVRDUDE=avrdude $(AVRDUDEFLAGS)

$(shell mkdir -p $(OBJDIR) >/dev/null)
$(shell mkdir -p $(BINDIR) >/dev/null)

SOURCES=$(shell find $(SRCDIR) -type f -name '*.c')
OBJECTS+=$(SOURCES:$(SRCDIR)/%.c=$(OBJDIR)/%.o)

all: $(BINDIR)/$(TARGET).hex

$(OBJDIR)/vendor/%.o: $(VENDORDIR)/%.c
$(OBJDIR)/%.o: $(SRCDIR)/%.c
	@mkdir -p $(@D)
	$(COMPILE) $< -o $@

$(OBJDIR)/%.S: $(SRCDIR)/%.c
	@mkdir -p $(@D)
	$(COMPILEASM) $< -o $@

$(BINDIR)/$(TARGET).elf: $(OBJECTS)
	$(LINK) $^ -o $@

$(BINDIR)/%.hex: $(BINDIR)/%.elf
	avr-objcopy -j .text -j .data -O ihex $< $@
	avr-size --format=avr --mcu=$(MCU) $<

$(BINDIR)/%.objdump: $(BINDIR)/%.elf
	avr-objdump -D -S -s $< > $@

dump: $(BINDIR)/$(TARGET).objdump

flash: $(BINDIR)/$(TARGET).hex
	$(AVRDUDE) -U flash:w:$<

read-fuse:
	$(AVRDUDE) -U lfuse:r:-:h -U hfuse:r:-:h -U efuse:r:-:h

clean:
	rm -rf $(OBJDIR) $(BINDIR)

.PHONY: all dump flash read-fuse clean
