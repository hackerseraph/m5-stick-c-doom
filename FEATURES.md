# DOOM for M5StickC Plus - Feature Roadmap

## Current Features ✅

### Core Gameplay
- ✅ Real-time 3D raycasting engine
- ✅ First-person perspective
- ✅ Smooth 30 FPS rendering
- ✅ 8x8 tile-based world map
- ✅ Collision detection

### Graphics
- ✅ Double-buffered rendering (120x68 → 240x135)
- ✅ Sky and floor rendering
- ✅ Distance-based wall shading
- ✅ Sprite-based enemy rendering
- ✅ Real-time HUD display

### Controls
- ✅ Accelerometer tilt for rotation
- ✅ Button-based movement
- ✅ Shooting mechanics
- ✅ Pause system

### Enemies
- ✅ 5 simultaneous enemies
- ✅ AI pathfinding toward player
- ✅ Melee combat system
- ✅ Health tracking per enemy
- ✅ Enemy defeat scoring

### UI/UX
- ✅ Health bar display
- ✅ Ammo counter
- ✅ Score tracking
- ✅ FPS counter
- ✅ Menu system
- ✅ Game over screen

## Planned Enhancements 🚀

### Phase 1: Polish (Easy)
- [ ] Muzzle flash effect when shooting
- [ ] Screen shake on hit
- [ ] Death animation for enemies (fade out)
- [ ] Victory screen when all enemies defeated
- [ ] Sound effects using M5 speaker (shoot, hit, death)
- [ ] Crosshair in center of screen
- [ ] Better enemy sprite (simple bitmap)
- [ ] Wall texture variation based on map value
- [ ] Minimap in corner

### Phase 2: Gameplay (Medium)
- [ ] Multiple weapon types (pistol, shotgun, chaingun)
- [ ] Power-ups (health packs, ammo boxes)
- [ ] Doors that can open/close
- [ ] Keys and locked doors (red, blue, yellow)
- [ ] Different enemy types (imp, demon, cacodemon)
- [ ] Ranged enemy attacks (fireballs)
- [ ] Damage indication (red screen flash)
- [ ] Weapon switching
- [ ] Secret areas

### Phase 3: Content (Medium)
- [ ] Level progression (multiple maps)
- [ ] Save/load game state to EEPROM
- [ ] High score table
- [ ] Difficulty levels (easy, normal, hard)
- [ ] Boss enemies
- [ ] Story/mission objectives
- [ ] Achievements system
- [ ] Time attack mode

### Phase 4: Advanced (Hard)
- [ ] Textured walls (real texture mapping)
- [ ] Floor and ceiling textures
- [ ] Multi-height walls (steps, platforms)
- [ ] Animated wall textures (water, lava)
- [ ] Particle effects (blood, explosions)
- [ ] Multiple weapon fire modes
- [ ] Multiplayer via WiFi (cooperative)
- [ ] Level editor accessible from device
- [ ] SD card support for custom maps
- [ ] Music playback

## Technical Improvements 🔧

### Performance
- [ ] Assembly optimization for raycasting inner loop
- [ ] PSRAM utilization for larger frame buffer
- [ ] Second CPU core for AI processing
- [ ] Lookup tables for trig functions
- [ ] Fixed-point math instead of floats
- [ ] Reduce memory allocations

### Graphics
- [ ] Bilinear filtering for smoother scaling
- [ ] Variable render resolution based on FPS
- [ ] Color palette optimization
- [ ] Dithering for more colors
- [ ] Fog effect for distant objects

### Code Quality
- [ ] Separate files for each module
- [ ] Better memory management
- [ ] Save game compression
- [ ] Map format specification
- [ ] Configuration file on SD card

## Community Wishlist 💭

Ideas from potential users:

- [ ] WAD file loader (actual Doom maps)
- [ ] Cheat codes (IDDQD, IDKFA)
- [ ] Speedrun timer
- [ ] Replay recording/playback
- [ ] Custom texture packs
- [ ] Mod support
- [ ] Online leaderboards
- [ ] Bluetooth controller support

## Performance Goals 🎯

| Metric | Current | Target |
|--------|---------|--------|
| FPS | 25-35 | 35-50 |
| Input lag | ~30ms | <20ms |
| RAM usage | ~20KB | <30KB |
| Flash usage | 331KB | <500KB |
| Boot time | 2s | <1s |

## Map Ideas 💡

### Level Themes
1. **Tech Base** (current) - Gray walls, tech facility
2. **Hell** - Red walls, lava floor
3. **City** - Urban environment, buildings
4. **Fortress** - Stone castle, medieval
5. **Space Station** - Metal corridors, windows to space
6. **Underground** - Cave walls, dark atmosphere
7. **Cathedral** - Gothic architecture
8. **Factory** - Industrial machinery

### Map Sizes
- Small: 8x8 (current)
- Medium: 16x16
- Large: 32x32
- Huge: 64x64 (if memory permits)

## Enemy Types 👹

### Tier 1 (Easy)
- **Zombie** - Slow, 30 HP, melee only
- **Imp** - Medium speed, 50 HP (current), basic fireball

### Tier 2 (Medium)
- **Demon** - Fast, 150 HP, strong melee
- **Cacodemon** - Flying, 100 HP, rapid fire
- **Hell Knight** - Tough, 200 HP, powerful attacks

### Tier 3 (Hard)
- **Baron of Hell** - Boss, 500 HP, multiple attacks
- **Arch-Vile** - Resurrects enemies, 300 HP
- **Cyberdemon** - Final boss, 1000 HP, rockets

## Weapons 🔫

### Tier 1
- **Fist** - Melee, infinite use
- **Pistol** - Hitscan, weak, infinite ammo
- **Shotgun** - Close range, strong, 2 ammo

### Tier 2
- **Chaingun** - Rapid fire, medium damage, 1 ammo/shot
- **Rocket Launcher** - Splash damage, 5 ammo
- **Plasma Gun** - Fast projectile, 2 ammo

### Tier 3
- **BFG 9000** - Massive damage, 40 ammo
- **Chainsaw** - Melee, infinite, close range instakill

## Code Snippets for Features 📝

### Muzzle Flash
```cpp
void drawMuzzleFlash() {
  uint32_t flashTime = millis();
  if (flashTime - lastShotTime < 50) {
    M5.Lcd.fillCircle(120, 67, 20, YELLOW);
  }
}
```

### Health Pack Pickup
```cpp
struct Pickup {
  float x, y;
  int type; // 0=health, 1=ammo
  bool active;
};

void checkPickups() {
  for (int i = 0; i < MAX_PICKUPS; i++) {
    if (!pickups[i].active) continue;
    float dx = playerX - pickups[i].x;
    float dy = playerY - pickups[i].y;
    if (sqrt(dx*dx + dy*dy) < 0.5f) {
      if (pickups[i].type == 0) playerHealth = min(100.0f, playerHealth + 25);
      if (pickups[i].type == 1) playerAmmo += 10;
      pickups[i].active = false;
    }
  }
}
```

### Minimap
```cpp
void drawMinimap() {
  int mmX = 190, mmY = 90;
  int mmScale = 5;
  
  for (int y = 0; y < mapHeight; y++) {
    for (int x = 0; x < mapWidth; x++) {
      uint16_t color = worldMap[y][x] ? WHITE : BLACK;
      M5.Lcd.fillRect(mmX + x*mmScale, mmY + y*mmScale, mmScale, mmScale, color);
    }
  }
  
  // Player dot
  M5.Lcd.fillCircle(mmX + playerX*mmScale, mmY + playerY*mmScale, 2, GREEN);
}
```

## Contributing 🤝

Want to add a feature? Here's how:

1. Fork the project
2. Create a feature branch
3. Implement and test
4. Submit pull request
5. Add documentation

## License 📄

All enhancements should maintain MIT license compatibility.

---

**Status**: Active Development
**Version**: 1.0.0
**Last Updated**: December 14, 2025
