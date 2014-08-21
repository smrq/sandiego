#!/bin/bash
set -e
echo "Please enter DFU mode."
read -p "Press [Enter] key to continue..."
sudo ./flash.sh firmware/Arduino-usbserial-uno.hex
read -p "Press [Enter] key to continue..."
sudo ino upload
echo "Please enter DFU mode."
read -p "Press [Enter] key to continue..."
sudo ./flash.sh firmware/Keyboard/Keyboard.hex