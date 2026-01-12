# 🎮 BUILD SUCCESS! - M5StickC Plus GBA DOOM

## ✅ What We Accomplished

### 1. Project Built Successfully
```
RAM:   [=         ]   7.3% (used 23916 bytes from 327680 bytes)
Flash: [====      ]  39.5% (used 802781 bytes from 2031616 bytes)
Build time: 1.90 seconds
```

### 2. WAD Flashed to Device
```bash
./flash_wad.sh /dev/ttyUSB0
```
- File: doom1.wad (1.2MB)
- Location: 0x210000 (flash partition)
- Status: ✅ Successfully flashed

### 3. Firmware Uploaded
```bash
./build.sh upload
```
- Size: 803KB
- RAM usage: 7.3% (23.9KB)
- Status: ✅ Running stable

### 4. Device Running
```
Free heap: 351KB
Status: Stable, no crash loops
```

## 📁 Project Structure

We're using **Sprite_tm's ESP32-C3 approach**:

### WAD Storage
- ✅ WAD stored in flash partition (NOT compiled in)
- ✅ Memory-mapped at runtime (saves RAM)
- ✅ Original doom1.wad (1.3MB) - no GbaWadUtil needed

### Code Base
- ✅ GBADoom source from Sprite_tm's repo
- ✅ Custom partition table (`partitions_gba_doom.csv`)
- ✅ M5StickC Plus LCD/input drivers

## 🔧 Build Commands

### Flash Everything:
```bash
# One-time WAD flash
./flash_wad.sh /dev/ttyUSB0

# Build and upload firmware
./build.sh upload

# Monitor serial output
./build.sh monitor
```

### Quick Commands:
```bash
./build.sh build          # Compile only
./build.sh upload         # Build + upload firmware
./build.sh all            # Build + upload + monitor
./build.sh full /dev/ttyUSB0   # Flash WAD + upload + monitor
```

## 📊 Memory Status

### Current (Simple Test):
```
Total RAM: 320KB
System: ~100KB
Available: ~220KB
M5 overhead: minimal (no library loaded yet)
Free heap: 351KB (includes IRAM)
```

### When DOOM Loads:
```
DOOM needs: ~250KB
Current available: ~200KB (with M5 library)
Deficit: ~50KB ⚠️
```

## ⚠️ Known Issues

### 1. Memory Shortage
- Sprite_tm had 400KB RAM (ESP32-C3)
- We have 320KB RAM (ESP32-PICO)
- Shortfall: **80KB less than Sprite_tm**

### 2. M5 Library Overhead
- M5StickCPlus library: ~20KB
- Arduino framework: overhead
- Total extra: ~20-30KB vs Sprite_tm's baremetal

### 3. Current Status
- ✅ Simple test runs perfectly
- ⏳ Full DOOM init not tested yet (switched to simple test)
- ⏳ Need to optimize for 50-80KB less RAM

## 🎯 Next Steps

### Option 1: Test Full DOOM
Switch back to `main_manual_init.cpp` or `main_gba.cpp`:
```bash
# Edit platformio.ini:
# Change: +<main_simple_test.cpp>
# To:     +<main_manual_init.cpp>

./build.sh upload
```

### Option 2: Reduce Memory Usage
Before running full DOOM:
1. Strip down M5 library (use bare ST7789 driver)
2. Reduce DOOM buffers (MAXVISPLANES, VISSPRITES)
3. Consider baremetal approach (no Arduino framework)

### Option 3: Test Incrementally
Add DOOM init functions one at a time:
1. ✅ WAD mapping (DONE - working!)
2. Input init
3. LCD init
4. Z_Init (memory zones)
5. D_DoomMain

## 📝 Files Created/Modified

### New Files:
- `flash_wad.sh` - Flash WAD to device
- `monitor.sh` - Monitor serial output
- `src/main_simple_test.cpp` - Simple WAD test
- `SPRITE_TM_APPROACH.md` - Documentation
- `BUILD_SUCCESS.md` - This file

### Modified:
- `build.sh` - Added flash-wad, all, full commands
- `platformio.ini` - Using main_simple_test.cpp

## 🔍 Debugging

### View Serial Output:
```bash
./build.sh monitor
```

### Reset Device:
```bash
~/.platformio/penv/bin/python \
    ~/.platformio/packages/tool-esptoolpy/esptool.py \
    --chip esp32 --port /dev/ttyUSB0 --after hard_reset read_mac
```

### Check Flash:
```bash
# Read WAD partition
~/.platformio/penv/bin/python \
    ~/.platformio/packages/tool-esptoolpy/esptool.py \
    --chip esp32 --port /dev/ttyUSB0 \
    read_flash 0x210000 16 wad_header.bin

xxd wad_header.bin
# Should show: IWAD at start
```

## 🎉 Success Metrics

- ✅ Project compiles
- ✅ Firmware uploads
- ✅ Device runs stable
- ✅ WAD accessible in flash
- ✅ 351KB free heap
- ⏳ DOOM gameplay (not tested yet)

## 📚 References

- Sprite_tm's ESP32-C3 Doom: https://github.com/Spritetm/esp32c3-doom-bauble
- GBADoom original: https://github.com/doomhack/GBADoom
- Our docs: `SPRITE_TM_APPROACH.md`, `GBA_DOOM_MIGRATION.md`

---

**Status:** Ready to attempt full DOOM initialization!  
**Date:** December 20, 2025  
**Build:** Successful ✅
