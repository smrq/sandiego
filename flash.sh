#!/bin/bash
if [[ $# -ne 1 ]]; then
	echo "Usage: flash.sh file.hex"
	exit 1
fi

if [[ $UID -ne 0 ]]; then
	echo "Must be run as root"
	exit 1
fi

dfu-programmer atmega16u2 erase &&
dfu-programmer atmega16u2 flash --debug 1 $1 &&
dfu-programmer atmega16u2 reset &&
echo Unplug and replug the USB cable now.
