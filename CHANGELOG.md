# Changelog - DOOM for M5StickC Plus 1.1

All notable changes to this project will be documented in this file.

## [1.0.0] - 2025-12-14

### 🎉 Initial Release

#### Added
- **Complete 3D raycasting engine** based on DDA algorithm
- **Playable game loop** with menu, gameplay, pause, and game over states
- **Enemy AI system** with pathfinding toward player
- **Combat mechanics** with shooting and melee systems
- **HUD display** showing health, ammo, score, and FPS
- **Tilt-based controls** using MPU6886 accelerometer
- **8x8 world map** with collision detection
- **5 simultaneous enemies** with health tracking
- **Scoring system** (100 points per enemy kill)
- **Resource management** (health and ammo)
- **Double-buffered rendering** for smooth 30 FPS
- **Distance-based shading** for depth perception
- **Sprite-based enemy rendering**
- **Complete documentation** (README, QUICKSTART, FEATURES)
- **Build automation script** (build.sh)

#### Technical Details
- Firmware size: 332 KB (10.8% flash usage)
- RAM usage: 22.5 KB (6.9% RAM usage)
- Render resolution: 120x68 internal, 2x scaled to 240x135
- Target FPS: 30-35
- Code: 569 lines (main + config)
- Platform: ESP32 @ 240MHz
- Framework: Arduino
- Library: M5StickCPlus 0.1.1

#### Controls
- Button A: Shoot weapon
- Button B (Hold): Move forward
- Button B (Long Press): Pause game
- Tilt Left/Right: Rotate view

#### Game Features
- Player stats: 100 HP, 50 ammo
- Enemy stats: 50 HP, 2 damage per hit
- Wall collision detection
- Raycast-based shooting with hit detection
- Game over on death
- Menu system with splash screen

#### Build System
- PlatformIO configuration with PSRAM support
- Huge app partition scheme for larger programs
- Debug level optimizations disabled
- M5StickCPlus library dependency
- Build script with commands: build, upload, monitor, clean, all

#### Documentation
- README.md: Complete feature documentation
- QUICKSTART.md: Quick start guide for new users
- FEATURES.md: Feature roadmap with 50+ enhancement ideas
- PROJECT_SUMMARY.md: Technical overview and statistics
- Inline code comments throughout

### Known Limitations
- No audio (memory constraints)
- Simple enemy sprites (solid rectangles)
- Fixed 8x8 map size
- No texture mapping
- Single weapon type
- No power-ups or items
- Simplified AI (straight-line pathfinding)

### Performance Notes
- Runs at 25-35 FPS on M5StickC Plus 1.1
- ~30ms input latency
- 2-second boot time
- Stable memory usage (no leaks)

## Future Versions

### [1.1.0] - Planned
- [ ] Muzzle flash effect
- [ ] Death animations for enemies
- [ ] Crosshair display
- [ ] Victory screen
- [ ] Sound effects

### [1.2.0] - Planned
- [ ] Multiple levels
- [ ] Power-ups (health, ammo)
- [ ] Different enemy types
- [ ] Texture-mapped walls

### [2.0.0] - Planned
- [ ] Save/load system
- [ ] High score table
- [ ] Difficulty levels
- [ ] Boss battles

## Development Notes

### Build Information
- Developed with PlatformIO
- Tested on M5StickC Plus 1.1
- Arduino framework 3.20017
- ESP32 platform 6.12.0

### Code Statistics
```
Language                     files          blank        comment           code
-------------------------------------------------------------------------------
C++                              1             65             40            547
C Header                         1              4              1             22
-------------------------------------------------------------------------------
SUM:                             2             69             41            569
```

### Compilation Output
```
RAM:   [=         ]   6.9% (used 22596 bytes from 327680 bytes)
Flash: [=         ]  10.8% (used 338989 bytes from 3145728 bytes)
Build time: 3.67 seconds
```

## Credits

Created by: Marcus
Date: December 14, 2025
License: MIT

Inspired by:
- DOOM (id Software, 1993)
- Wolfenstein 3D (id Software, 1992)
- Classic raycasting algorithms
- M5Stack community projects

## Links

- Hardware: [M5StickC Plus](https://shop.m5stack.com/products/m5stickc-plus-esp32-pico-mini-iot-development-kit)
- Platform: [PlatformIO](https://platformio.org/)
- Framework: [Arduino-ESP32](https://github.com/espressif/arduino-esp32)

---

**Note**: This is version 1.0.0 - the initial complete and working release!
