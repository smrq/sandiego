CC=avr-gcc

SRCDIR=src
DEPDIR=dep
OBJDIR=obj
BINDIR=bin

SHAREDDIR=$(abspath $(dir $(lastword $(MAKEFILE_LIST))))
VENDORDIR=$(abspath $(SHAREDDIR)/../vendor)

# NOTE: For whatever reason, the Teensy ArduinoISP seems to change tty every
# time it's plugged in.  On my system, it seems to always be the second
# tty.usbmodem* entry, so I just figure out what that is dynamically.
# If there's a way of determining this more robustly, that would be great.
PORT:=$(shell ls /dev/tty.usbmodem* | sed -n '2 p')

DEPFLAGS+=-MT $@ -MMD -MP -MF $(DEPDIR)/$*.temp-d
CFLAGS+=-Wall -Wextra -Werror=implicit-function-declaration -std=gnu11 -I$(VENDORDIR) -I$(SHAREDDIR)
OPTFLAGS+=-O2 -flto
ARCHFLAGS+=-mmcu=$(MCU) -DF_CPU=$(CPU)
AVRDUDEFLAGS+=-C $(SHAREDDIR)/avrdude.conf -p $(MCU) -c arduino -P $(PORT) -v

COMPILE=$(CC) $(DEPFLAGS) $(CFLAGS) $(OPTFLAGS) $(ARCHFLAGS) -c
COMPILEVENDOR=$(CC) $(CFLAGS) $(OPTFLAGS) $(ARCHFLAGS) -c
COMPILEASM=$(CC) $(CFLAGS) -O2 $(ARCHFLAGS) -S
LINK=$(CC) $(OPTFLAGS) $(ARCHFLAGS)
AVRDUDE=avrdude $(AVRDUDEFLAGS)

$(shell mkdir -p $(DEPDIR) >/dev/null)
$(shell mkdir -p $(OBJDIR) >/dev/null)
$(shell mkdir -p $(BINDIR) >/dev/null)

SOURCES=$(wildcard $(SRCDIR)/*.c)
OBJECTS+=$(SOURCES:$(SRCDIR)/%.c=$(OBJDIR)/%.o)

all: $(BINDIR)/$(TARGET).hex

$(OBJDIR)/vendor/%.o: $(VENDORDIR)/%.c
	@mkdir -p $(@D)
	$(COMPILEVENDOR) $< -o $@

$(OBJDIR)/%.o: $(SRCDIR)/%.c
$(OBJDIR)/%.o: $(SRCDIR)/%.c $(DEPDIR)/%.d
	@mkdir -p $(@D)
	$(COMPILE) $< -o $@
	@mv -f $(DEPDIR)/$*.temp-d $(DEPDIR)/$*.d && touch $@

$(OBJDIR)/%.S: $(SRCDIR)/%.c
	@mkdir -p $(@D)
	$(COMPILEASM) $< -o $@

$(DEPDIR)/%.d: ;

$(BINDIR)/$(TARGET).elf: $(OBJECTS)
	$(LINK) $^ -o $@

$(BINDIR)/%.hex: $(BINDIR)/%.elf
	avr-objcopy -j .text -j .data -O ihex $< $@
	avr-size --format=avr --mcu=$(MCU) $<

$(BINDIR)/%.objdump: $(BINDIR)/%.elf
	avr-objdump -D -S -s $< > $@

include $(wildcard $(patsubst %,$(DEPDIR)/%.d,$(basename $(SOURCES))))

dump: $(BINDIR)/$(TARGET).objdump

flash: $(BINDIR)/$(TARGET).hex
	$(AVRDUDE) -U flash:w:$<

read-fuse:
	$(AVRDUDE) -U lfuse:r:-:h -U hfuse:r:-:h -U efuse:r:-:h

clean:
	rm -rf $(OBJDIR) $(BINDIR) $(DEPDIR)

.PHONY: dump flash read-fuse clean
.PRECIOUS: $(DEPDIR)/%.d
