// doomgeneric configuration for M5StickC Plus
// Aggressive memory optimizations for 320KB RAM

#ifndef DOOMGENERIC_CONFIG_H
#define DOOMGENERIC_CONFIG_H

// Use 8-bit indexed color mode to save memory
#define CMAP256 1

// Screen resolution - match M5StickC Plus native resolution  
#define DOOMGENERIC_RESX 135
#define DOOMGENERIC_RESY 240

// Disable features we don't have hardware for
#define FEATURE_SOUND 0
#define FEATURE_MUSIC 0

// CRITICAL: Reduce zone memory to absolute minimum
// Doom needs heap for sprites, textures, levels etc
// We're severely constrained, so go minimal
#define ZONE_MEMORY (64 * 1024)  // Only 64KB for zone (very tight!)

// Disable network play
#define NO_NETWORK 1

// Disable demo recording to save memory
#define NO_DEMO_RECORDING 1

// Use simpler rendering mode
#define HIGH_DETAIL 0
#define LOW_DETAIL 1

// Reduce screen blocks for better performance
#define DEFAULT_SCREENBLOCKS 9

// Reduce save game buffer
#define SAVEGAMESIZE (64 * 1024)  // Reduce from default

#endif // DOOMGENERIC_CONFIG_H
