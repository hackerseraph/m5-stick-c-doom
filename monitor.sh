#!/bin/bash
# Monitor and capture boot sequence

echo "Resetting device and capturing boot..."
cd /home/marcus/Documents/PlatformIO/Projects/M5-Stick-C-Plus-Doom

# Reset device
~/.platformio/penv/bin/python ~/.platformio/packages/tool-esptoolpy/esptool.py \
    --chip esp32 --port /dev/ttyUSB0 --after hard_reset read_mac > /dev/null 2>&1

# Wait and monitor
sleep 1
timeout 5 ~/.platformio/penv/bin/platformio device monitor -e m5stick-c-plus-gba --baud 115200 2>&1 | tee boot_log.txt
