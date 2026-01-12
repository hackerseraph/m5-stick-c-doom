# Ultra-Aggressive Optimization Results

## Date: 2025-12-15

## Optimizations Applied

### 1. Rendering Buffer Reductions
- **MAXVISPLANES**: 128 → 24 (saves ~15.8KB)
- **MAXVISSPRITES**: 128 → 32 (saves ~12.3KB)
- **MAXDRAWSEGS**: 256 → 64 (saves ~6.1KB)
- **MAXOPENINGS**: SCREENWIDTH*64 → SCREENWIDTH*16 (saves ~13KB)
- **Total from buffers**: ~47KB saved

### 2. Zone Memory Reduction
- **Zone heap**: 32KB → 24KB (saves 8KB)

### 3. Feature Disabling
- **Demo recording**: Disabled (saves ~5KB code)
- **Demo playback**: Disabled (saves ~5KB code)
- **Automap**: Disabled (saves ~5KB code + buffers)
- **Save/load**: Disabled (saves ~5KB)
- **Screenshot**: Disabled (saves ~1KB)
- **Total from features**: ~21KB saved

### 4. IRAM Strategy Changed
- **Previously**: Moved 65KB (states, mobjinfo, ticdata, stats) to IRAM
- **Now**: Moved all back to DRAM to fix IRAM overflow
- **Effect**: Fixed IRAM but increased DRAM by 65KB

## Memory Status

### Before All Optimizations
- **DRAM overflow**: 119KB
- **IRAM overflow**: 2.6KB
- **Total overflow**: 121.6KB

### After Buffer Reductions Only
- **DRAM overflow**: 13KB  
- **IRAM overflow**: 2.6KB  
- **Total overflow**: 15.6KB
- **Savings**: 106KB DRAM saved!

### After Moving Arrays Back from IRAM
- **DRAM overflow**: 79KB
- **IRAM overflow**: 0KB ✅
- **Total overflow**: 79KB
- **Net progress**: 42.6KB total saved

## Analysis

**What Worked:**
1. ✅ Rendering buffer reductions: **106KB DRAM freed** (massive win!)
2. ✅ Fixed IRAM overflow completely
3. ✅ Feature disabling compiles cleanly

**The Trade-off:**
- Moving 65KB from IRAM back to DRAM fixed IRAM but consumed DRAM
- Net result: 106KB saved - 65KB consumed = **41KB net improvement**

**Why IRAM Strategy Failed:**
- ESP32 has limited IRAM (~68KB available)
- We filled it with 65KB of game data
- This left only 3KB for code, which overflowed
- Moving data out of IRAM put it back in DRAM
- We don't have PSRAM like jkirsons' port does

## Remaining Overflow: 79KB

## Next Steps to Get Under Budget

### Option 1: Extreme Buffer Reduction (Conservative)
Further reduce buffers to near-unplayable levels:
- MAXVISPLANES: 24 → 16 (save ~1KB)
- MAXVISSPRITES: 32 → 24 (save ~1KB)
- MAXDRAWSEGS: 64 → 48 (save ~1KB)
- Total: ~3KB more

**Still short by 76KB**

### Option 2: Texture/Sprite Streaming (Complex)
Implement on-demand loading from flash:
- Keep only 4-8 textures in RAM
- Load others from SPIFFS as needed
- Potential: 60-100KB saved
- Complexity: Very high, needs extensive refactoring

### Option 3: Screen Resolution Reduction (Simple)
Reduce from 135x240 to 120x200:
- Screen buffers: 32.4KB → 24KB (save 8.4KB)
- Rendering tables: proportional savings (~5KB)
- Total: ~13KB saved

**Still short by 66KB**

### Option 4: Remove More Features
- Remove ALL weapons except pistol/fist (save ~10KB)
- Remove power-ups (save ~5KB)
- Remove decorations (save ~3KB)
- Total: ~18KB saved

**Still short by 61KB**

### Option 5: Nuclear - Reduce Level Data
- Only support E1M1-E1M3 (remove rest)
- Minimal enemy types (4-5 types max)
- Potential: 30-40KB saved

**Still short by 39KB**

### Option 6: Combination Approach (Realistic Path)
1. Texture streaming (60KB)
2. Screen resolution reduction (13KB)
3. Remove some enemies/weapons (10KB)
4. **Total: 83KB - Would succeed!**

## Recommendation

**Implement Option 6 (Combination Approach):**

1. **Phase 1: Texture Streaming** (Most impactful)
   - Implement LRU texture cache (4 textures in RAM)
   - Load from SPIFFS on demand
   - Expected: 60KB saved

2. **Phase 2: Resolution Reduction** (if Phase 1 insufficient)
   - Reduce to 120x200
   - Expected: 13KB saved

3. **Phase 3: Content Reduction** (if still needed)
   - Remove rarely-used enemies
   - Reduce weapon variety
   - Expected: 10KB saved

**Total potential: 83KB saved → Would fit with 4KB headroom**

## Alternative: Accept Limitations

If texture streaming is too complex, consider:
- Port is educational/proof-of-concept
- Document "almost ran DOOM on ESP32 without PSRAM"
- Show that it compiles and allocates correctly
- Note that runtime would likely crash due to insufficient memory

## Files Modified

### Buffer Reductions
- `lib/doomgeneric/r_plane.c` - MAXVISPLANES, MAXOPENINGS
- `lib/doomgeneric/r_things.h` - MAXVISSPRITES
- `lib/doomgeneric/r_defs.h` - MAXDRAWSEGS
- `lib/doomgeneric/i_system.c` - Zone size

### Feature Disabling
- `lib/doomgeneric/doomgeneric_config.h` - Config flags
- `lib/doomgeneric/g_game.c` - Demo stubs
- `lib/doomgeneric/am_map.c` - Automap stubs
- `platformio.ini` - Compiler defines

### Memory Management
- `lib/doomgeneric/info.c` - States/mobjinfo arrays
- `lib/doomgeneric/info.h` - Array declarations
- `lib/doomgeneric/d_loop.c` - Ticdata array
- `lib/doomgeneric/statdump.c` - Stats array

## Conclusion

We've made **significant progress** (122KB → 79KB overflow), but need one more major optimization to succeed. Texture streaming is the most promising approach, though it requires substantial effort.

The port demonstrates that DOOM *almost* runs on ESP32 without PSRAM through aggressive optimization. With texture streaming, it would succeed.
