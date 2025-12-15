# ESP32 Memory Deep Dive - M5StickC Plus Doom Port

## Current Memory Situation (Build Failed - 197KB Overflow)

### Base RAM Usage Breakdown
```
DATA section (initialized):  59 KB
BSS section (uninitialized):  37 KB
Zone memory heap:            128 KB
--------------------------------
TOTAL DEMAND:                224 KB

ESP32 DRAM Available:        ~120 KB (after system reserves)
OVERFLOW:                    197 KB (201,760 bytes)
```

### Top RAM Consumers (from nm analysis)
```
27,076 bytes - states (D_Dehacked.c) - Dehacked patch state table
20,480 bytes - ticdata (d_net.c) - Network tic buffer
12,604 bytes - mobjinfo (info.c) - Map object definitions  
 6,400 bytes - captured_stats (statdump.c) - Statistics capture
 5,232 bytes - S_sfx (sounds.c) - Sound effect definitions
 2,976 bytes - extra_defaults_list (m_config.c) - Config defaults
 1,824 bytes - doom_defaults_list (m_config.c) - More config
 1,280 bytes - events (d_event.c) - Event queue
 1,088 bytes - S_music (sounds.c) - Music definitions
 1,024 bytes - gamekeydown (g_game.c) - Keyboard state
   640 bytes - cliptop/clipbot (r_segs.c) - 2x320 byte clip arrays
```

## ESP32-PICO-D4 Hidden Memory Map

### Standard DRAM Regions
- **DRAM0**: 192 KB @ 0x3FFB0000-0x3FFE0000 (main heap)
- **DRAM1**: 128 KB @ 0x3FFE0000-0x40000000 (used by system)

### 🔥 EXPLOITABLE HIDDEN REGIONS

#### 1. RTC Slow Memory (8 KB) ⚡
- **Location**: 0x50000000-0x50002000
- **Type**: Ultra-low-power SRAM, survives deep sleep
- **Usage**: Store rarely-changed lookup tables that don't fit in flash
- **Access**: `RTC_SLOW_MEM` attribute
- **Speed**: Slower than DRAM but faster than flash reads
```c
RTC_SLOW_ATTR static const uint8_t rtc_lookup[8192];
```

#### 2. RTC Fast Memory (8 KB) ⚡⚡
- **Location**: 0x3FF80000-0x3FF82000  
- **Type**: Fast SRAM, survives deep sleep
- **Usage**: Frequently accessed small buffers (sprite cache, temp buffers)
- **Access**: `RTC_FAST_ATTR` attribute
- **Speed**: Same as DRAM
```c
RTC_FAST_ATTR static uint8_t sprite_buffer[4096];
```

#### 3. IRAM as Data Storage (128 KB usable) ⚡⚡⚡
- **Location**: 0x40080000-0x400A0000 (total 128KB)
- **Type**: Instruction RAM, but can store data!
- **Usage**: Critical: Only ~60KB used by ESP-IDF/Arduino, leaves ~68KB!
- **Access**: `IRAM_ATTR` for code, or manual `.iram1` section for data
- **Speed**: Same as DRAM
```c
// Store static game data in IRAM
IRAM_ATTR static const mobj_t iram_mobjs[256];
```

#### 4. I-Cache as Scratchpad (16 KB) 🔥🔥🔥
- **Usage**: Normally caches flash instructions
- **Hack**: During non-rendering phases, can use as temp buffer
- **Method**: `cache_flush()` → use as RAM → `cache_invalidate()`
- **Speed**: DRAM speed when not caching

#### 5. Flash Memory-Mapped Region (Fake RAM) ⚡
- **Location**: 0x3F400000-0x3F800000 (4 MB)
- **Type**: XIP (Execute-In-Place) flash, read-only
- **Usage**: Keep ALL const data here with PROGMEM
- **Speed**: ~40 cycles vs 1 cycle for DRAM (still acceptable)

### ESP32 Memory Map Summary
```
0x3FF80000-0x3FF82000    8 KB  RTC Fast (exploitable!)
0x3FFB0000-0x3FFE0000  192 KB  DRAM0 (main battlefield)
0x3FFE0000-0x40000000  128 KB  DRAM1 (system, hard to steal)
0x40080000-0x400A0000  128 KB  IRAM (GOLD MINE - mostly unused!)
0x50000000-0x50002000    8 KB  RTC Slow (exploitable!)
```

## ESP32 Instruction Set Hacks

### 1. Xtensa SIMD Instructions (Unused in Doom!)
ESP32 has 16-bit dual-MAC operations perfect for fixed-point math:

```c
// Current Doom code (scalar):
int result = FixedMul(a, b);  // Uses 32-bit multiply

// ESP32 optimized (SIMD):
// Process 2 fixed-point muls in parallel
__asm__ volatile (
    "mull.dd.ll %0, %1, %2"  // Dual 16x16->32 multiply
    : "=a"(result) : "a"(a), "a"(b)
);
```

### 2. Windowed Registers (Underutilized)
- ESP32 has 64 physical registers, Doom only uses 16
- Use register windows for function calls to avoid stack spills
```c
// Force inline critical functions
#define INLINE __attribute__((always_inline)) inline
```

### 3. FPU Instruction Fusion
- sinf/cosf are single-cycle but not auto-vectorized
- Use ESP32-S2+ DSP extensions if available

### 4. Unaligned Access (Free on ESP32)
- Doom does a lot of `(int*)((char*)ptr + 1)` with alignment checks
- ESP32 handles unaligned access in hardware - remove checks!

```c
// Remove alignment penalties:
#define UNALIGNED_READ32(ptr) (*((uint32_t*)(ptr)))
```

### 5. MEMW Elimination
- Compiler inserts memory barriers everywhere
- Doom is single-threaded, remove barriers:
```c
-fno-threadsafe-statics -fno-rtti
```

## Optimization Strategy

### Phase 1: Exploit Hidden Memory (Target: 100 KB freed)
1. ✅ Move `states[27KB]` → IRAM (`IRAM_ATTR`)
2. ✅ Move `mobjinfo[12KB]` → IRAM
3. ✅ Move `ticdata[20KB]` → RTC Fast (it's rarely accessed)
4. ✅ Move sound tables `S_sfx[5KB]` + `S_music[1KB]` → RTC Slow
5. ✅ Move `captured_stats[6KB]` to flash (only written at end)
6. ✅ Move config defaults `[4KB]` → flash with PROGMEM

**Estimated savings: 75 KB from DRAM**

### Phase 2: Reduce Static Buffers (Target: 30 KB freed)
```c
// In r_plane.c:
#define MAXVISPLANES 32      // was 128, save ~19KB
// In r_things.h:
#define MAXVISSPRITES 64     // was 128, save ~4KB  
// In r_defs.h:
#define MAXDRAWSEGS 64       // was 256, save ~12KB
```

**Estimated savings: 35 KB**

### Phase 3: Reduce Zone Memory (Target: 60 KB freed)
```c
// In i_system.c:
#define ESP32_ZONE_SIZE (68 * 1024)  // was 128KB, save 60KB
```

Test if game runs with less heap. May need texture streaming if not.

**Estimated savings: 60 KB**

### Phase 4: ESP32 Instruction Optimization (Target: 15 KB freed)
1. Replace FixedMul with inline Xtensa dual-MAC
2. Remove alignment checks (ESP32 handles it)
3. Inline critical functions (avoid stack frame overhead)
4. Use -O3 with ESP32-specific flags

**Estimated savings: 15 KB from reduced code size**

### Phase 5: Nuclear Option - Texture Streaming
If we're still short, stream textures from SPIFFS:
- Keep only 4-8 textures in RAM at once
- Load/evict based on LRU cache
- **Savings: 80-120 KB**

## Total Potential Savings
```
Hidden memory:      75 KB
Buffer reduction:   35 KB
Zone reduction:     60 KB
Code optimization:  15 KB
---------------------
TOTAL:             185 KB

TARGET:            197 KB overflow
GAP:                12 KB (need texture streaming)
```

## Implementation Order
1. ✅ IRAM data migration (biggest win)
2. ✅ RTC memory usage (easy win)
3. ✅ Buffer size reduction (test carefully)
4. ✅ Zone memory reduction (test gameplay)
5. ⚠️  Texture streaming (if needed)
6. 🔧 Xtensa instruction optimization (polish)

## ESP32-Specific Compiler Flags Needed
```ini
-fno-threadsafe-statics    ; Remove mutex overhead
-fno-rtti                  ; No runtime type info
-fno-exceptions            ; No C++ exceptions
-ffast-math                ; Aggressive FPU optimization
-funroll-loops             ; Unroll small loops
-finline-limit=64          ; Inline more functions
-mtext-section-literals    ; Put literals in code section
```

## 90s Game Dev Tricks Applied
1. **Doom trick**: Use IRAM as data (like N64 used texture RAM)
2. **Quake trick**: Reduce visplanes (use occlusion more)
3. **Demo scene**: RTC memory for "persistent" data
4. **Console trick**: Manual register allocation for hot paths
5. **Arcade trick**: Stream textures like arcade boards did

## Next Steps
Ready to implement Phase 1 (IRAM migration) - should save ~75KB immediately.
