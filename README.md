# DOOM for M5StickC Plus - Extreme Memory Optimization Journey

Port of actual id Software DOOM (not a raycasting demo) to the M5StickC Plus 1.1 using aggressive ESP32 memory hacking techniques.

**Status:** 🚧 IN PROGRESS - Close but not yet playable  
**Challenge:** Fitting a 4MB RAM game into 320KB with NO PSRAM

## Hardware Target
- **Device**: M5StickC Plus 1.1
- **MCU**: ESP32-PICO-D4
- **CPU**: Dual-core Xtensa @ 240MHz
- **RAM**: 320KB SRAM total (~120KB available after system reserves)
- **Flash**: 4MB
- **Display**: ST7789 135x240 RGB565
- **PSRAM**: ❌ NONE (this is the challenge!)

## The Challenge

Original DOOM was designed for 4-8MB RAM. We have **120KB DRAM available**. Initial port overflowed by **201KB**.

## Our Hacks & Optimizations

### 1. Sound System: COMPLETELY STRIPPED ✅
- **Saved**: ~6KB RAM + eliminated processing overhead
- **Rationale**: M5StickC has no speaker
- **Implementation**: Replaced sounds.c with minimal stubs (2 dummy entries)

### 2. ESP32 Hidden Memory Exploitation ✅
- **IRAM Usage**: 65KB of game data moved to instruction RAM
- **Moved**: states (27KB), mobjinfo (12KB), ticdata (20KB), stats (6KB)
- **Result**: IRAM NOW FULL (overflowing by 2.6KB)

### 3. Calculated Trigonometry ✅
- Replaced 64KB lookup tables with ESP32 FPU real-time calculation
- Kept tantoangle/gammatable in flash (PROGMEM)

### 4. Zone Memory Reduction ✅
- Original: 128KB heap → Current: 32KB heap
- **Saved**: 96KB

### 5. Compiler Optimizations ✅
- -O3, -ffast-math, -funroll-loops, dead code elimination

## Current Status

### Memory Usage
```
DRAM Overflow: 119 KB ❌
IRAM Overflow: 2.6 KB ❌
```

### Progress from Start
- Started: 201KB overflow
- After optimizations: 119KB DRAM + 2.6KB IRAM overflow
- **Total saved**: 82KB (but hit IRAM limit!)

## Current Blockers

1. **IRAM Exhausted** - Maxed out at 65KB of data
2. **DRAM Still 119KB Over** - Need more aggressive cuts

## Not Yet Attempted

- Reduce rendering buffers (MAXVISPLANES/VISSPRITES/DRAWSEGS) - ~35KB potential
- Texture streaming from flash - ~60-100KB potential
- Screen buffer optimization - ~10-30KB potential
- Disable features (demos, saves, network) - ~5-15KB potential
- ESP32 SIMD assembly optimization - ~5-10KB potential

## File Structure
```
lib/doomgeneric/          # Full DOOM engine
├── esp32_mem_hack.h      # Memory exploitation layer
├── sounds.c              # STRIPPED - minimal stubs
├── info.c                # states/mobjinfo → IRAM
└── [93 other files]
src/main.cpp              # M5StickC display code
MEMORY_ANALYSIS.md        # Deep ESP32 memory dive
```

## Build
```bash
git clone https://github.com/hackerseraph/m5-stick-c-doom.git
cd m5-stick-c-doom
pio run  # Currently fails with overflow
```

## What Makes This Different

This is ACTUAL id Software DOOM code:
- ✅ Full BSP rendering engine
- ✅ Real Doom sprites/textures
- ✅ Authentic gameplay logic
- ❌ NOT a raycaster demo
- ❌ NOT simplified engine

## Credits
- id Software - Original DOOM
- doomgeneric - Cross-platform framework
- 90s demo scene - Inspiration

**"Rip and tear... through memory limits!"**
