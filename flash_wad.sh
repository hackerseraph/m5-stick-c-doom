#!/bin/bash
# Flash WAD file to ESP32 partition
# Based on Sprite_tm's esp32c3-doom-bauble approach

# Configuration
WAD_FILE="data/doom1.wad"
WAD_OFFSET=0x210000  # Must match partitions_gba_doom.csv
PORT="${1:-/dev/ttyUSB0}"
PYTHON="$HOME/.platformio/penv/bin/python"
ESPTOOL="$HOME/.platformio/packages/tool-esptoolpy/esptool.py"

echo "=========================================="
echo "  Flash WAD to M5StickC Plus"
echo "=========================================="
echo ""

# Check if WAD exists
if [ ! -f "$WAD_FILE" ]; then
    echo "ERROR: WAD file not found: $WAD_FILE"
    exit 1
fi

WAD_SIZE=$(stat -c%s "$WAD_FILE")
echo "WAD file: $WAD_FILE"
echo "WAD size: $WAD_SIZE bytes ($(echo "scale=2; $WAD_SIZE/1024/1024" | bc) MB)"
echo "Flash offset: $WAD_OFFSET"
echo "Port: $PORT"
echo ""

# Check partition size (1.5MB = 1572864 bytes)
MAX_SIZE=1572864
if [ $WAD_SIZE -gt $MAX_SIZE ]; then
    echo "ERROR: WAD too large! Max size is 1.5MB"
    exit 1
fi

echo "Flashing WAD to device..."
"$PYTHON" "$ESPTOOL" --chip esp32 --port "$PORT" --baud 115200 \
    --before default_reset --after hard_reset write_flash "$WAD_OFFSET" "$WAD_FILE"

if [ $? -eq 0 ]; then
    echo ""
    echo "✅ WAD flashed successfully!"
    echo ""
    echo "Now run: ./build.sh upload"
else
    echo ""
    echo "❌ Flash failed!"
    exit 1
fi
