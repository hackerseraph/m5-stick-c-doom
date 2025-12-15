# Quick Start Guide - DOOM for M5StickC Plus 1.1

## Getting Started in 3 Steps

### 1. Build the Project
```bash
cd /home/marcus/Documents/PlatformIO/Projects/M5-Stick-C-Plus-Doom
./build.sh build
```

### 2. Upload to Device
```bash
# Connect your M5StickC Plus via USB, then:
./build.sh upload
```

### 3. Play!
- Press **Button A** to start from the menu
- **Hold Button B** to move forward
- **Tilt the device** left/right to turn
- **Press Button A** to shoot enemies

## Build Script Commands

```bash
./build.sh build      # Compile only
./build.sh upload     # Build and flash to device
./build.sh monitor    # Open serial monitor
./build.sh clean      # Clean build files
./build.sh all        # Build, upload, and monitor
```

## Game Controls

| Input | Action |
|-------|--------|
| **Button A** | Shoot weapon |
| **Button B (Hold)** | Move forward |
| **Button B (Long 1s)** | Pause game |
| **Tilt Left/Right** | Rotate/turn |

## Gameplay Tips

1. **Start**: From menu, press Button A to begin
2. **Navigate**: Tilt device to look around, hold B to walk forward
3. **Combat**: Face enemies and press A to shoot (uses 1 ammo)
4. **Survival**: Avoid getting too close to red enemies (they deal melee damage)
5. **Resources**: Watch your health bar (top-left) and ammo count (top-right)

## HUD Elements

- **Top-left**: Green health bar (starts at 100 HP)
- **Top-right**: Yellow ammo counter (A:50)
- **Top-center**: White score display (S:0)
- **Bottom-right**: Green FPS counter

## Troubleshooting

### Build Issues
- Ensure PlatformIO is installed: `~/.platformio/penv/bin/platformio --version`
- Run `./build.sh clean` then `./build.sh build`

### Upload Issues
- Check USB connection (use data cable, not charge-only)
- Press and hold reset button during upload if needed
- Try different USB port

### Performance Issues
- Normal FPS: 25-35 FPS
- If laggy, reduce enemy count or render resolution in `include/doom_config.h`

### Control Issues
- Ensure device is held horizontally (landscape)
- Calibrate by holding device level when starting
- Tilt sensitivity can be adjusted in code (search for `accelY` multiplier)

## Advanced Customization

Edit `include/doom_config.h`:

```cpp
#define RENDER_WIDTH 120    // Higher = better quality, lower FPS
#define RENDER_HEIGHT 68    // Higher = better quality, lower FPS
#define MAX_DOOM_TICKS 35   // Lower = higher FPS target
#define MAX_ENEMIES 5       // More enemies = harder game
```

Edit `src/main.cpp` world map to create custom levels:

```cpp
const uint8_t worldMap[8][8] = {
  {1,1,1,1,1,1,1,1},
  {1,0,0,0,0,0,0,1},
  // ... modify walls (1) and spaces (0)
};
```

## Next Steps

- See `README.md` for complete documentation
- Check `FEATURES.md` for planned enhancements
- Modify enemy spawn positions in `spawnEnemies()` function
- Create larger maps by changing `mapWidth` and `mapHeight`

## Support

Built on December 14, 2025 for M5StickC Plus 1.1

Enjoy your pocket-sized DOOM experience! 🎮
