#ifndef _constants_h
#define _constants_h

#include <Arduino.h>

// M5StickC Plus buttons  
#define K_FIRE    37  // Button A
#define K_UP      39  // Button B

// GFX settings - adapted for M5StickC Plus ST7789 240x135
#define FRAME_TIME          50.0        // 20 fps target (faster than Arduino)
#define RES_DIVIDER         2           // Resolution divider for performance
#define Z_RES_DIVIDER       2           // Zbuffer resolution divider
#define DISTANCE_MULTIPLIER 20          // Distance precision multiplier
#define MAX_RENDER_DEPTH    12
#define MAX_SPRITE_DEPTH    8

#define ZBUFFER_SIZE        (SCREEN_WIDTH / Z_RES_DIVIDER)

// Level 
#define LEVEL_WIDTH_BASE    6
#define LEVEL_WIDTH         (1 << LEVEL_WIDTH_BASE)  // 64
#define LEVEL_HEIGHT        57
#define LEVEL_SIZE          (LEVEL_WIDTH / 2 * LEVEL_HEIGHT)

// Scenes
#define INTRO               0
#define GAME_PLAY           1

// Game constants
#define GUN_TARGET_POS        18
#define GUN_SHOT_POS          (GUN_TARGET_POS + 4)

#define ROT_SPEED             0.12
#define MOV_SPEED             0.2
#define MOV_SPEED_INV         5           // 1 / MOV_SPEED

#define JOGGING_SPEED         0.005
#define ENEMY_SPEED           0.02
#define FIREBALL_SPEED        0.2
#define FIREBALL_ANGLES       45          // Num of angles per PI

#define MAX_ENTITIES          10          // Max num of active entities
#define MAX_STATIC_ENTITIES   28          // Max num of entities in sleep mode
#define MAX_ENTITY_DISTANCE   200         // * DISTANCE_MULTIPLIER
#define MAX_ENEMY_VIEW        80          // * DISTANCE_MULTIPLIER
#define ITEM_COLLIDER_DIST    6           // * DISTANCE_MULTIPLIER
#define ENEMY_COLLIDER_DIST   4           // * DISTANCE_MULTIPLIER
#define FIREBALL_COLLIDER_DIST 2          // * DISTANCE_MULTIPLIER
#define ENEMY_MELEE_DIST      6           // * DISTANCE_MULTIPLIER
#define WALL_COLLIDER_DIST    0.2

#define ENEMY_MELEE_DAMAGE    8
#define ENEMY_FIREBALL_DAMAGE 20
#define GUN_MAX_DAMAGE        15

// Display - M5StickC Plus ST7789 TFT
constexpr uint8_t SCREEN_WIDTH     = 160;   // Reduced from 240 for performance
constexpr uint8_t SCREEN_HEIGHT    = 90;    // Reduced from 135 for performance  
constexpr uint8_t RENDER_WIDTH     = 80;    // Rendering resolution
constexpr uint8_t RENDER_HEIGHT    = 45;    // Rendering resolution
constexpr uint8_t PIXEL_SCALE      = 2;     // Scale factor
constexpr uint8_t HALF_WIDTH       = SCREEN_WIDTH / 2;
constexpr uint8_t HALF_HEIGHT      = SCREEN_HEIGHT / 2;

// Frame timing
#define MAX_DOOM_TICKS      40      // Target FPS

// Color palette for M5StickC Plus (RGB565)
#define COLOR_BLACK     0x0000
#define COLOR_WHITE     0xFFFF
#define COLOR_RED       0xF800
#define COLOR_GREEN     0x07E0
#define COLOR_BLUE      0x001F
#define COLOR_YELLOW    0xFFE0
#define COLOR_GRAY      0x8410
#define COLOR_DARKGRAY  0x4208

#define GRADIENT_COUNT  4   // Number of wall shading gradients

#endif
