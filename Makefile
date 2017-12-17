TARGET=main
CC=avr-gcc
MCU=attiny88
CPU=8000000

COMPILEFLAGS=-Wall -Wextra $(OPTFLAGS) -mmcu=$(MCU) -DF_CPU=$(CPU) -std=gnu11
OPTFLAGS=-O2 -flto
CFLAGS=$(COMPILEFLAGS) $(OPTFLAGS)

SRCDIR = src
OBJDIR = obj
BINDIR = bin

SOURCES = $(wildcard $(SRCDIR)/*.c)
OBJECTS = $(SOURCES:$(SRCDIR)/%.c=$(OBJDIR)/%.o)

all: $(BINDIR)/$(TARGET).hex

$(OBJDIR):
	mkdir $(OBJDIR)

$(BINDIR):
	mkdir $(BINDIR)

$(OBJDIR)/%.d: $(SRCDIR)/%.c | $(OBJDIR)
	$(CC) -MM $(CFLAGS) $< | sed 's,\($*\)\.o[ :]*,$(OBJDIR)/\1.o $@ : ,g' > $@

$(OBJDIR)/%.o : $(SRCDIR)/%.c $(OBJDIR)/%.d
	$(CC) -c $(CFLAGS) $< -o $@

$(OBJDIR)/%.S: $(SRCDIR)/%.c | $(OBJDIR)
	$(CC) -S $(COMPILEFLAGS) $< -o $@

$(BINDIR)/$(TARGET).elf: $(OBJECTS) | $(BINDIR)
	$(CC) $(CFLAGS) $(OBJECTS) -o $@

$(BINDIR)/%.hex: $(BINDIR)/%.elf
	avr-objcopy -j .text -j .data -O ihex $< $@
	avr-size --format=avr --mcu=$(MCU) $<

include $(wildcard $(OBJDIR)/*.d)

clean:
	rm -rf $(OBJDIR) $(BINDIR)

.PHONY: clean
.PRECIOUS: $(OBJDIR)/%.d
