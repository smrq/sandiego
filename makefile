# Default target
all: build firmware

clean:
	ino clean
	$(MAKE) -C firmware/Keyboard clean

build: clean
	ino build

firmware:
	$(MAKE) -C firmware/Keyboard

upload: build firmware
	./upload.sh

.PHONY: all build clean firmware upload