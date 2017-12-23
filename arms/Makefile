TARGET=main
CC=avr-gcc
MCU=attiny88
CPU=8000000

SRCDIR = src
DEPDIR = dep
OBJDIR = obj
BINDIR = bin

# NOTE: For whatever reason, the Teensy ArduinoISP seems to change tty every
# time it's plugged in.  On my system, it seems to always be the second
# tty.usbmodem* entry, so I just figure out what that is dynamically.
# If there's a way of determining this more robustly, that would be great.
PORT:=$(shell ls /dev/tty.usbmodem* | sed -n '2 p')

DEPFLAGS=-MT $@ -MMD -MP -MF $(DEPDIR)/$*.temp-d
CFLAGS=-Wall -Wextra -std=gnu11 -I$(abspath ../shared)
OPTFLAGS=-O2 -flto
ARCHFLAGS=-mmcu=$(MCU) -DF_CPU=$(CPU)

COMPILE=$(CC) $(DEPFLAGS) $(CFLAGS) $(OPTFLAGS) $(ARCHFLAGS) -c
COMPILEASM=$(CC) $(DEPFLAGS) $(CFLAGS) $(ARCHFLAGS) -S
POSTCOMPILE=@mv -f $(DEPDIR)/$*.temp-d $(DEPDIR)/$*.d && touch $@
LINK=$(CC) $(OPTFLAGS) $(ARCHFLAGS)

$(shell mkdir -p $(DEPDIR) >/dev/null)
$(shell mkdir -p $(OBJDIR) >/dev/null)
$(shell mkdir -p $(BINDIR) >/dev/null)

SOURCES = $(wildcard $(SRCDIR)/*.c)
OBJECTS = $(SOURCES:$(SRCDIR)/%.c=$(OBJDIR)/%.o)

all: $(BINDIR)/$(TARGET).hex

$(OBJDIR)/%.o: $(SRCDIR)/%.c
$(OBJDIR)/%.o: $(SRCDIR)/%.c $(DEPDIR)/%.d
	$(COMPILE) $< -o $@
	$(POSTCOMPILE)

$(OBJDIR)/%.S: $(SRCDIR)/%.c
$(OBJDIR)/%.S: $(SRCDIR)/%.c $(DEPDIR)/%.d
	$(COMPILEASM) $< -o $@
	$(POSTCOMPILE)

$(DEPDIR)/%.d: ;

$(BINDIR)/$(TARGET).elf: $(OBJECTS)
	$(LINK) $(OBJECTS) -o $@

$(BINDIR)/%.hex: $(BINDIR)/%.elf
	avr-objcopy -j .text -j .data -O ihex $< $@
	avr-size --format=avr --mcu=$(MCU) $<

include $(wildcard $(patsubst %,$(DEPDIR)/%.d,$(basename $(SOURCES))))

flash: $(BINDIR)/$(TARGET).hex
	avrdude -v -p attiny88 -c arduino -P $(PORT) -U flash:w:$<

clean:
	rm -rf $(OBJDIR) $(BINDIR) $(DEPDIR)

.PHONY: clean flash
.PRECIOUS: $(DEPDIR)/%.d
