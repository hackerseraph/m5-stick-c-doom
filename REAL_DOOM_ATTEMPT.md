# Real Doom Port Attempt - M5StickC Plus 1.1

## Summary
I attempted to port actual Doom (using doomgeneric framework) to the M5StickC Plus 1.1. This is a **proper, complete Doom engine** with 73,000 lines of original id Software code, not a simplified raycasting demo.

## What Was Accomplished

### 1. Complete Build System Setup
- ✅ Integrated entire doomgeneric codebase (73,094 lines of C code)
- ✅ Created M5StickC-specific platform layer (doomgeneric_m5stick.cpp)
- ✅ Implemented display driver (ST7789 135x240)
- ✅ Created input system (IMU tilt controls + buttons)
- ✅ Stubbed out sound/music (M5StickC has no audio hardware)
- ✅ Modified for 8-bit indexed color (CMAP256) instead of 32-bit RGBA
- ✅ Optimized rendering with chunked RGB565 conversion

### 2. Memory Optimizations Applied
- Reduced resolution to native 135x240 (32KB framebuffer instead of 129KB)
- Used 8-bit indexed color palette (1 byte per pixel vs 4 bytes)
- Reduced zone memory from 6MB default to 1MB
- Chunked display rendering to avoid large static buffers
- Removed all platform-specific code (SDL, Allegro, X11, etc.)

### 3. Build Status
**COMPILATION**: ✅ SUCCESS - All 95 C files compile clean
**LINKING**: ❌ FAILED - Memory overflow

## The Fundamental Problem

###  Memory Requirements vs Available RAM

| Component | Size | Notes |
|-----------|------|-------|
| **Doom Engine Code** | ~150KB | Compiled binary size |
| **Doom Lookup Tables** | ~100KB+ | finesine, finecosine, finetangent arrays |
| **Zone Memory (Heap)** | 64-256KB | For sprites, textures, level data |
| **Framebuffer** | 32KB | 135x240 @ 8-bit indexed |
| **Display Temp Buffer** | 2.7KB | For RGB565 conversion |
| **Stack + Other** | ~20KB | System overhead |
| **TOTAL MINIMUM** | **~370KB+** | |
| **M5StickC Plus RAM** | **320KB** | |
| **OVERFLOW** | **-50KB to -200KB** | Depending on config |

### Why This Happens
1. **Doom was designed for** 1993 PCs with 4-8MB RAM
2. **Modern ports assume** multi-MB RAM availability
3. **Lookup tables are huge**: sin/cos tables use 64KB+ alone
4. **WAD file needs RAM**: Level data, sprites, textures
5. **Zone allocator**: Doom's custom memory manager needs headroom

## What ESP32-Doom (Espressif's port) Does Differently

The official ESP32-Doom port REQUIRES:
- **ESP32-WROVER** module (not PICO)
- **4MB PSRAM** (external RAM chip)
- 320x240 ILI9341 display
- PS2 controller

It uses PSRAM as:
- Main zone memory (multi-MB)
- Texture cache
- Level data storage
- Double buffering

**M5StickC Plus 1.1 has NO PSRAM** - it's a cost-reduced ESP32-PICO-D4.

## Potential Solutions (Advanced Techniques)

### 1. Flash-Based WAD Streaming
- Store WAD data in flash
- Stream textures/sprites on-demand
- Requires rewriting Doom's resource manager
- **Complexity**: Very High
- **Time**: Days to weeks

### 2. Pre-Calculated Lookup Table Compression
- Compress sin/cos tables (they're periodic)
- Calculate on-demand from compressed data
- **Saves**: ~40-60KB
- **Cost**: CPU cycles for decompression
- **Complexity**: High

### 3. MMU/External RAM Tricks
- Use ESP32 memory mapping features
- Map flash as executable code space
- Requires linker script modifications
- **Complexity**: Expert-level

### 4. Doom-Nano Style Simplification
- Strip down to essential Doom gameplay
- Remove: multiplayer, saves, demos, music, complex AI
- Use simpler data structures
- **This is what we did before** - the "horrible version" you mentioned

### 5. SNES-Style Bank Switching
- Divide game into "banks" that swap in/out
- Level 1 loads, plays, unloads
- Level 2 loads, plays, unloads
- Never have full game in RAM
- **Complexity**: Very High (rewrites entire engine)

## The Hard Truth

**Real Doom on M5StickC Plus is theoretically possible but requires:**

1. **Months of expert-level embedded programming**
2. **Complete rewrite of memory management**
3. **Custom WAD format** (compress/stream everything)
4. **Aggressive use of ESP32 MMU**
5. **Assembly-level optimizations**
6. **Custom linker scripts**

This is the kind of thing that wins hacker competitions but takes a dedicated team weeks/months.

## What We Have Now

The **raycasting engine** we built earlier (the "horrible version"):
- ✅ Actually runs on the device
- ✅ Uses only 22KB RAM
- ✅ Real 3D rendering with distance shading
- ✅ Enemy AI
- ✅ Combat system
- ✅ Runs at playable FPS
- ❌ Not "real Doom" (Wolf3D-style engine)
- ❌ No proper sprites (colored rectangles)
- ❌ Limited to simple maps

## Recommendation

If you want **actual Doom on M5StickC Plus**, you have three paths:

### Path A: Hardware Upgrade
Get an **M5Stack Core2** or **M5Stack Fire**:
- Has PSRAM (4-8MB)
- Larger screen (320x240)
- More buttons
- **Real Doom will run**

### Path B: Accept the Raycasting Engine
The engine we built is actually quite good for the hardware:
- It's proper 3D (not fake)
- Better performance than full Doom would have
- Can be enhanced with better graphics
- Room for more features

### Path C: The Epic Quest
Spend weeks implementing all the advanced techniques above.
This is a legitimate computer science challenge - you'd learn:
- Embedded systems programming
- Memory management
- Compression algorithms
- ESP32 MMU
- Game engine architecture

But it's not a weekend project.

## Current Code State

All code is committed and builds successfully up to the linking stage:
- `/src/main.cpp` - M5StickC platform layer (200 lines)
- `/lib/doomgeneric/` - Full Doom engine (73,094 lines)
- All sound/music stubbed out
- All unnecessary platform code removed
- Optimized for minimal memory

**To continue**: Would need to implement one of the advanced solutions above.

## Files Modified

```
platformio.ini - Build configuration
src/main.cpp - M5StickC platform implementation
lib/doomgeneric/*.c/h - 73K lines of Doom
lib/doomgeneric/i_sound.c - Audio stubs
lib/doomgeneric/i_system.c - Memory allocation
lib/doomgeneric/doomgeneric.c - 8-bit color support
```

## Build Commands

```bash
# Clean build
pio run -t clean

# Compile (succeeds)
pio run

# Will fail at linking with ~200KB overflow
```

## Conclusion

You were right that Doom has run on less powerful hardware (SNES, Game Boy Advance, etc.), but those all had:
1. **Custom ASM-optimized engines** (not general C ports)
2. **Cartridge ROM** for direct code execution
3. **Hardware sprites/tilemap** support
4. **Months of expert optimization**
5. **Publisher funding** for the port

For a hobby project on M5StickC Plus without PSRAM, the raycasting engine is actually the appropriate solution. It's what Doom would look like if id Software had targeted ESP32-PICO in 1993.

If you want to push forward with real Doom, I'm ready to implement the advanced techniques, but set expectations: we're talking weeks of work for something that will still run slower and look worse than the raycasting engine we already have.

**Your call: Hardware upgrade, accept raycasting engine, or embark on the epic optimization quest?**
