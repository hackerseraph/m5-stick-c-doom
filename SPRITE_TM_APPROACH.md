# Sprite_tm ESP32-C3 Doom Approach

## Overview
This document explains how Sprite_tm successfully ran GBADoom on ESP32-C3 and how we're adapting it for M5StickC Plus.

## Key Differences

### Hardware
| Feature | Sprite_tm (ESP32-C3) | M5StickC Plus (ESP32-PICO) | Impact |
|---------|---------------------|---------------------------|---------|
| **RAM** | 400KB | 320KB | ⚠️ We have 80KB LESS |
| **Flash** | 4MB | 4MB | ✅ Same |
| **PSRAM** | None | None | ✅ Same |
| **Display** | 240x240 GC9A01 | 240x135 ST7789 | ✅ Smaller is better |

## Sprite_tm's Build Process

### 1. WAD Preparation
Sprite_tm uses the **original doom1.wad** (1.3MB), NOT a GbaWadUtil-converted version:
```bash
# Flash WAD to custom partition
esptool.py --chip esp32 --port /dev/ttyUSB0 --baud 921600 \
    write_flash 0x210000 doom1.wad
```

### 2. Memory Mapping (NOT Compilation)
Unlike GBADoom for GBA (which compiles WAD into ROM), Sprite_tm:
- ✅ Stores WAD in flash partition
- ✅ Memory-maps it at runtime using `esp_partition_mmap()`
- ✅ Saves RAM by keeping WAD in flash address space
- ❌ Does NOT use GbaWadUtil

This is CRITICAL - the WAD never enters RAM, it's accessed directly from flash.

### 3. Partition Table
```csv
# Name,     Type, SubType, Offset,   Size,    Flags
nvs,        data, nvs,     0x9000,   0x4000,
phy_init,   data, phy,     0xD000,   0x1000,
factory,    app,  factory, 0x10000,  0x200000,
wad,        66,   6,       0x210000, 0x180000,
```

**Note:** 
- Factory app: 2MB (ESP32-C3 has more flash for firmware)
- WAD partition: 1.5MB (same as ours)

### 4. Our Adaptation

#### What We Copied:
✅ GBADoom source code from Sprite_tm's repo  
✅ Partition mapping approach  
✅ Custom partition table  
✅ Input/LCD abstraction layer  

#### What We Changed:
🔧 **LCD driver**: GC9A01 (240x240) → ST7789 (240x135)  
🔧 **Input**: ESP32-C3 buttons → M5StickC buttons + IMU  
🔧 **Framework**: ESP-IDF → Arduino + M5StickC library  
🔧 **Firmware size**: 2MB → 1.96MB (less space, same code)  

#### What We're Fighting:
⚠️ **80KB less RAM** than ESP32-C3  
⚠️ M5 library overhead (~20KB)  
⚠️ Arduino framework overhead  

## Build Commands

### First Time Setup:
```bash
# 1. Flash WAD to device
./flash_wad.sh /dev/ttyUSB0

# 2. Build and flash firmware
./build.sh upload

# 3. Monitor serial output
./build.sh monitor
```

### Quick Rebuild:
```bash
# Build + upload + monitor
./build.sh all

# Everything including WAD
./build.sh full /dev/ttyUSB0
```

## Memory Budget

### Sprite_tm's ESP32-C3:
```
Total RAM:    400KB
System:       ~100KB (WiFi/BT disabled)
Available:    ~300KB
DOOM needs:   ~250KB (fits with margin)
```

### Our M5StickC Plus:
```
Total RAM:    320KB
System:       ~100KB (WiFi/BT disabled)
M5 library:   ~20KB
Available:    ~200KB
DOOM needs:   ~250KB (DOESN'T FIT!)
```

**The Problem:** We need 50KB more optimizations than Sprite_tm did.

## Critical Differences from GBADoom README

The GBADoom README describes building for **GBA hardware**, which is different:

| Feature | GBA Build | ESP32 Build (Sprite_tm) |
|---------|-----------|------------------------|
| WAD Storage | Compiled into ROM | Flash partition |
| WAD Tool | GbaWadUtil required | Original WAD used |
| Build System | DevKitArm + make | ESP-IDF + CMake |
| Memory Map | Static in ROM | Dynamic mmap |
| Size Limit | 32MB ROM | 4MB flash |

**Bottom Line:** The GBADoom README's build instructions are for GBA cartridges, NOT ESP32. Sprite_tm adapted the SOURCE CODE but not the build process.

## What Actually Happens

### GBADoom (Original):
1. Run GbaWadUtil to create `doom.wad.c`
2. Include in source: `#include "iwad/doom.wad.c"`
3. Compile everything into one 32MB .gba file
4. WAD is in the ROM at compile-time

### Sprite_tm ESP32 (What We're Using):
1. Keep `doom1.wad` as separate file
2. Flash to custom partition at 0x210000
3. At runtime: `esp_partition_mmap()` to access
4. WAD stays in flash, accessed through memory mapping

## Why This Matters for Us

**We CANNOT use Sprite_tm's approach as-is because:**
1. He had 400KB RAM, we have 320KB
2. M5 library adds ~20KB overhead
3. We're 50-80KB short on memory

**Options:**
1. **Optimize M5 library usage** - strip down to bare ST7789 driver
2. **Reduce DOOM buffers** - smaller render buffers, fewer sprites
3. **Baremetal approach** - bypass Arduino framework entirely
4. **Upgrade hardware** - use ESP32-C3 or ESP32-S3 with more RAM

## Sprite_tm's Source

- GitHub: https://github.com/Spritetm/esp32c3-doom-bauble
- Video: https://www.youtube.com/watch?v=CCZf3EXrK4Q
- Based on GBADoom: https://github.com/doomhack/GBADoom

## Next Steps

1. ✅ Use Sprite_tm's partition mapping approach (DONE)
2. ✅ Flash WAD to device (script ready)
3. ⏳ Reduce memory usage by 50-80KB
4. ⏳ Test on actual hardware

---

**Key Takeaway:** We're using Sprite_tm's ESP32 adaptation method (not GBADoom's GBA build method), but we need extra optimizations because M5StickC has less RAM than ESP32-C3.
