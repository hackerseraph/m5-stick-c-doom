# GBA DOOM Migration - Implementation Guide

## Overview
Migrating from doomgeneric to GBA DOOM (Sprite_tm approach) for M5StickC Plus.

## Why GBA DOOM?
1. **Optimized for limited RAM**: Designed for 256KB GBA, perfect for ESP32's 320KB
2. **No filesystem overhead**: Memory-mapped flash access eliminates boot crashes
3. **Pre-computed structures**: doom1gba.wad has optimized fixed-point data
4. **Proven on ESP32**: Sprite_tm successfully ran it on ESP32-C3

## Completed Steps

### 1. Repository Setup ✅
- Cloned `doomhack/GBADoom` (base GBA DOOM port)
- Cloned `Spritetm/esp32c3-doom-bauble` (ESP32 reference implementation)

### 2. WAD Conversion ✅
- Built GbaWadUtil tool (Qt application)
- Converted stripped doom1.wad (1.2MB) → doom1gba.wad (1.3MB)
- Output: `/tmp/doom1gba.wad` (1,296,857 bytes)

### 3. Partition Table ✅
Created `partitions_gba_doom.csv`:
```
NVS:     0x9000  - 16KB   (settings)
PHY:     0xD000  - 4KB    (WiFi cal)
Factory: 0x10000 - 2MB    (firmware)
WAD:     0x210000- 1.5MB  (doom1gba.wad)
Total: 3.52MB of 4MB flash
```

### 4. GBA DOOM Integration ✅
- Copied 140 source files from esp32c3-doom to `lib/gbadoom/`
- Created `library.json` for PlatformIO
- Excluded ESP32-C3 specific files (lcd.c, input.c, snd_c3.c)

### 5. M5StickC Plus Hardware Abstraction ✅
Created custom drivers:
- **lcd_m5.c/h**: ST7789 display driver (240x135 framebuffer)
- **input_m5.c/h**: Button input (BtnA=Fire, BtnB=Use, BtnPWR=Menu)
- Uses M5StickCPlus library for hardware access

### 6. Main Entry Point ✅
Created `src/main_gba.cpp` with:
- Memory-mapped flash access via `esp_partition_mmap()`
- WAD verification (checks IWAD header)
- Hardware initialization sequence
- Calls GBA DOOM's `D_DoomMain()`

### 7. Build Configuration ✅
Updated `platformio.ini`:
- New environment: `m5stick-c-plus-gba`
- Uses `partitions_gba_doom.csv`
- GBA DOOM compiler flags from Sprite_tm
- Excludes old main.cpp

## Files Created/Modified

### New Files:
- `/lib/gbadoom/*.c, *.h` (140 files from GBA DOOM)
- `/lib/gbadoom/library.json`
- `/lib/gbadoom/lcd_m5.c` + `.h`
- `/lib/gbadoom/input_m5.c` + `.h`
- `/src/main_gba.cpp`
- `/partitions_gba_doom.csv`
- `/tmp/doom1gba.wad` (converted WAD)

### Modified:
- `platformio.ini` (new gba environment)

### Backed Up:
- `/lib/doomgeneric/` → `/lib/doomgeneric.old/`

## Next Steps (TODO)

### 1. Fix Compilation Issues
GBA DOOM likely has dependencies that need adaptation:
- Check for missing headers (config.h, etc.)
- Adapt i_video.c to use lcd_m5.h
- Adapt input handling to use input_m5.h
- Stub out sound code (dbopl.c, snd_c3.c)
- Fix global_data.h/c integration

### 2. I/O Layer Integration
Replace Sprite_tm's ESP32-C3 specific code:
- **i_video.c**: Change lcd.c calls → lcd_m5.c calls
- **i_input.c**: Change input.c calls → input_m5.c calls  
- **i_sound.c**: Stub out completely (no speaker)

### 3. Display Adaptation
Sprite_tm used different resolution, we need:
- Set `SCREENWIDTH=240, SCREENHEIGHT=135` (done in build flags)
- Ensure GBA DOOM rendering scales properly
- Verify framebuffer format (RGB565)

### 4. Build and Test
```bash
cd /home/marcus/Documents/PlatformIO/Projects/M5-Stick-C-Plus-Doom
pio run -e m5stick-c-plus-gba
```

Expected first build errors:
- Missing config.h (create stub)
- Undefined references to lcd.c functions
- Undefined references to input.c functions
- Linker issues with global_data

### 5. Flash WAD to Device
After firmware builds successfully:
```bash
# Flash firmware
pio run -e m5stick-c-plus-gba -t upload

# Flash WAD to partition 0x210000
esptool.py --chip esp32 --port /dev/ttyUSB0 \
  write_flash 0x210000 /tmp/doom1gba.wad
```

### 6. Debug Boot Issues
Monitor serial output:
```bash
pio device monitor -e m5stick-c-plus-gba
```

Watch for:
- WAD partition mapping success
- IWAD header verification
- Memory allocation issues
- Display initialization
- First frame rendering

## Key Differences from Original Approach

| Aspect | Old (doomgeneric) | New (GBA DOOM) |
|--------|------------------|----------------|
| Framework | Arduino | Arduino (no ESP-IDF needed!) |
| WAD Access | Filesystem (LittleFS) | Memory-mapped flash |
| WAD Format | Standard IWAD | doom1gba.wad (optimized) |
| RAM Usage | ~400KB needed | ~256KB designed |
| Boot | Crashed on mount | Direct mapping |
| Resolution | 60x108 | 240x135 (full screen!) |

## Critical Memory Mappings

```c
// In main_gba.cpp:
const esp_partition_t *wad_partition = esp_partition_find_first(66, 6, NULL);
esp_partition_mmap(wad_partition, 0, doom_iwad_len, 
                   SPI_FLASH_MMAP_DATA, &doom_iwad, &mmap_handle);
```

This maps flash directly to RAM address space - no copying, no filesystem!

## Expected Challenges

1. **Compilation errors**: GBA DOOM expects specific build environment
2. **Display integration**: Need to hook lcd_m5 into i_video.c properly
3. **Input mapping**: DOOM expects keyboard-like input, we have 3 buttons
4. **Performance**: GBA DOOM is fast, but ESP32 @240MHz should handle 240x135
5. **Memory**: 320KB should be enough, but watch heap usage

## Success Criteria

✅ Firmware compiles without errors  
✅ Device boots without crashes  
✅ WAD loads from flash partition  
✅ Display shows DOOM title screen  
✅ Buttons respond  
✅ Game plays at acceptable FPS  

## Resources

- Sprite_tm's blog: https://spritesmods.com/?art=doom-bauble
- GBA DOOM: https://github.com/doomhack/GBADoom
- ESP32-C3 version: https://github.com/Spritetm/esp32c3-doom-bauble
- GbaWadUtil: Tool in esp32c3-doom/GbaWadUtil/

## Current Status

**Phase**: Integration complete, ready for first build attempt  
**Next Action**: Run `pio run -e m5stick-c-plus-gba` and fix compilation errors  
**Confidence**: High - architecture proven on ESP32-C3, just needs adaptation
