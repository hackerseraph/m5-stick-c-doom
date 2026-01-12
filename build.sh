#!/bin/bash
# Build and Flash Script for M5StickC Plus DOOM

# Set the PlatformIO binary path
PIO=~/.platformio/penv/bin/platformio

# Change to project directory
cd "$(dirname "$0")"

echo "=================================="
echo "  DOOM for M5StickC Plus Builder  "
echo "=================================="
echo ""

# Parse arguments
case "$1" in
    "build")
        echo "Building project..."
        $PIO run
        ;;
    "upload")
        echo "Building and uploading to device..."
        $PIO run --target upload
        ;;
    "monitor")
        echo "Opening serial monitor..."
        $PIO device monitor
        ;;
    "clean")
        echo "Cleaning build files..."
        $PIO run --target clean
        ;;
    "flash-wad")
        echo "Flashing WAD file only..."
        ./flash_wad.sh "$2"
        ;;
    "all")
        echo "Building, uploading, and monitoring..."
        $PIO run --target upload && $PIO device monitor
        ;;
    "full")
        echo "Complete flash: WAD + firmware + monitor..."
        ./flash_wad.sh "$2" && $PIO run --target upload && $PIO device monitor
        ;;
    *)
        echo "Usage: $0 {build|upload|monitor|clean|flash-wad|all|full} [port]"
        echo ""
        echo "  build      - Compile the project"
        echo "  upload     - Build and flash firmware to M5StickC Plus"
        echo "  monitor    - Open serial monitor"
        echo "  clean      - Remove build files"
        echo "  flash-wad  - Flash WAD file only (specify port as 2nd arg)"
        echo "  all        - Build, upload firmware, and monitor"
        echo "  full       - Flash WAD, upload firmware, and monitor"
        echo ""
        echo "Example: $0 flash-wad /dev/ttyUSB0"
        echo "         $0 full /dev/ttyUSB0"
        echo ""
        exit 1
        ;;
esac
