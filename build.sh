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
    "all")
        echo "Building, uploading, and monitoring..."
        $PIO run --target upload && $PIO device monitor
        ;;
    *)
        echo "Usage: $0 {build|upload|monitor|clean|all}"
        echo ""
        echo "  build   - Compile the project"
        echo "  upload  - Build and flash to M5StickC Plus"
        echo "  monitor - Open serial monitor"
        echo "  clean   - Remove build files"
        echo "  all     - Build, upload, and monitor"
        echo ""
        exit 1
        ;;
esac
