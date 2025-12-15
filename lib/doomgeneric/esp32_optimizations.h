// ESP32 Memory Optimization for Doom
// Move large lookup tables to flash (PROGMEM) instead of RAM
// This saves ~64KB of precious DRAM

#ifndef ESP32_OPTIMIZATIONS_H
#define ESP32_OPTIMIZATIONS_H

#ifdef __cplusplus
extern "C" {
#endif

// ESP32 PROGMEM attribute - stores data in flash instead of RAM
#ifndef PROGMEM
#define PROGMEM __attribute__((section(".rodata")))
#endif

// Accessor macro for PROGMEM data on ESP32
#define pgm_read_dword(addr) (*((const uint32_t *)(addr)))
#define pgm_read_word(addr) (*((const uint16_t *)(addr)))
#define pgm_read_byte(addr) (*((const uint8_t *)(addr)))

#ifdef __cplusplus
}
#endif

#endif // ESP32_OPTIMIZATIONS_H
