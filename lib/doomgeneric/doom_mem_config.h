// Memory optimization configuration for ESP32
// Aggressive RAM reduction techniques

#ifndef DOOM_MEM_CONFIG_H
#define DOOM_MEM_CONFIG_H

// ==== OPTION 1: Eliminate lookup tables entirely ====
// Instead of 64KB of trig tables, calculate on-the-fly
// ESP32 @ 240MHz can do this MUCH faster than the 486 @ 66MHz Doom was built for
#define USE_CALCULATED_TRIG 1

// ==== OPTION 2: Compress static data ====
// Use LZ4 compression for large const arrays
// Decompress on-demand - trading CPU for RAM
#define USE_COMPRESSED_STRINGS 1

// ==== OPTION 3: Eliminate gamma table ====
// Use simple formula instead of 1.3KB lookup table
#define USE_CALCULATED_GAMMA 1

// ==== OPTION 4: Reduce buffer sizes ====
// Smaller temporary buffers, more frequent operations
#define MAX_VISPLANES 32    // Down from 128
#define MAX_VISSPRITES 64   // Down from 128
#define MAX_DRAWSEGS 64     // Down from 256

// ==== OPTION 5: Use stack instead of static buffers ====
#define USE_STACK_BUFFERS 1

// ==== OPTION 6: Streaming ====
// Load data from SPIFFS on-demand
#define USE_SPIFFS_STREAMING 1

// ==== OPTION 7: Dual-core utilization ====
// Use core 0 for rendering, core 1 for game logic
#define USE_DUAL_CORE 1

// ==== Memory limits ====
#define SCREENWIDTH 135
#define SCREENHEIGHT 240
#define MAXPLAYERS 1  // No multiplayer = save memory
#define MAXHEALTH 100
#define MAXARMOR 100

// Tiny zone size - we'll stream everything else
#define ZONE_SIZE (48 * 1024)  // Only 48KB! Down from 96KB

#endif // DOOM_MEM_CONFIG_H
