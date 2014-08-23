# Default target
all: build firmware

clean:
	ino clean
	$(MAKE) -C firmware/Keyboard clean

build: clean
	ino build -f="-Ishared/"

firmware:
	$(MAKE) -C firmware/Keyboard

upload: build firmware
	./upload.sh

firmup: firmware
	sudo ./flash.sh firmware/Keyboard/Keyboard.hex

.PHONY: all build clean firmware upload