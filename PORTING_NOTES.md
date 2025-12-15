# Porting Doom-Nano to M5StickC Plus

## Source
Based on QC20's Doom-Nano implementation for Arduino
https://github.com/QC20/Doom-run-on-Arduino-Pro-Mini-and-SSD1306

## Key Differences to Port:

### Display
- **Source**: SSD1306 OLED 128x64 monochrome I2C
- **Target**: ST7789 TFT 240x135 color SPI (M5StickC Plus)
- **Change**: Replace Adafruit_SSD1306 with M5.Lcd calls
- **Benefit**: Color support! Can use gradients properly

### Resolution
- **Source**: 128x64 (8KB monochrome buffer)
- **Target**: 240x135 → scale to 120x68 or 160x90 for performance
- **Memory**: ~16-30KB for RGB565 framebuffer

### Controls  
- **Source**: 5 digital buttons
- **Target**: 2 buttons + accelerometer tilt
- **Mapping**:
  - Button A = Fire
  - Button B = Forward
  - Tilt Left/Right = Turn
  - Button B long = Strafe/Back

### Performance
- **Source**: ATmega328P @ 16MHz, 2KB RAM
- **Target**: ESP32 @ 240MHz, 320KB RAM
- **Advantage**: 15x faster CPU, 160x more RAM!

## Files to Port (2482 lines total):
- doom-nano.ino - Main game loop
- display.h/cpp - Rendering engine (adapt for TFT)
- entities.h/cpp - Enemy/item logic
- input.h/cpp - Controls (adapt for M5 buttons + accel)
- level.h - Map data
- sprites.h - Graphics data  
- constants.h - Game configuration
- types.h/cpp - Data structures

## Rendering Strategy:
1. Keep the efficient raycasting algorithm
2. Replace SSD1306 buffer with RGB565 framebuffer
3. Add color gradients for walls/floor/ceiling
4. Use M5.Lcd.pushImage() for fast bulk transfers

This is REAL Doom-like gameplay with proper textures, enemies, keys, doors!
