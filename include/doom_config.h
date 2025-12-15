#ifndef DOOM_CONFIG_H
#define DOOM_CONFIG_H

// Display configuration for M5StickC Plus
#define SCREEN_WIDTH 240
#define SCREEN_HEIGHT 135
#define RENDER_WIDTH 80    // Optimized resolution
#define RENDER_HEIGHT 45   // Optimized resolution (3:1 ratio)

// Game configuration
#define MAX_DOOM_TICKS 40  // Target FPS (~25 FPS)
#define PIXEL_SCALE 3      // Scale factor (3x upscale)

// Controls (using M5 library button objects)
// Button A: Shoot
// Button B: Move forward (hold) / Pause (long press)

// Memory optimization
#define DOOM_TINY          // Use minimal memory mode
#define NO_SOUND           // Disable audio

#endif
