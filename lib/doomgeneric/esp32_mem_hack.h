#ifndef ESP32_MEM_HACK_H
#define ESP32_MEM_HACK_H

#include <stdint.h>

/**
 * ESP32 Hidden Memory Exploitation Layer
 * 
 * This header provides attributes to place data in ESP32's hidden memory regions:
 * - IRAM: 128KB instruction RAM (60-70KB available for data!)
 * - RTC Fast: 8KB fast SRAM (survives deep sleep)
 * - RTC Slow: 8KB slow SRAM (survives deep sleep, slower access)
 * 
 * Strategy: Move large const/rarely-modified data out of precious DRAM
 */

// ==== IRAM Exploitation ====
// IRAM is normally for code, but we can store data there!
// ESP32 has 128KB IRAM, only ~60KB used by ESP-IDF/Arduino
// Available: ~68KB for Doom data

#ifdef ESP32
  // Place large const arrays in IRAM instead of DRAM
  #define IRAM_DATA __attribute__((section(".iram1.data")))
  
  // Place functions in IRAM for speed (avoid flash cache miss)
  #define IRAM_FUNC __attribute__((section(".iram1.text")))
  
  // Force inline to save stack frame overhead (ESP32 has 64 regs!)
  #define FORCE_INLINE __attribute__((always_inline)) inline
  
#else
  #define IRAM_DATA
  #define IRAM_FUNC  
  #define FORCE_INLINE inline
#endif

// ==== RTC Memory Exploitation ====
// RTC Fast: 8KB, same speed as DRAM, survives deep sleep
// RTC Slow: 8KB, slower (40 vs 1 cycle), survives deep sleep

#ifdef ESP32
  // RTC Fast: For frequently accessed small buffers
  #define RTC_FAST_DATA __attribute__((section(".rtc.data")))
  
  // RTC Slow: For rarely accessed lookup tables
  #define RTC_SLOW_DATA __attribute__((section(".rtc.force_slow")))
#else
  #define RTC_FAST_DATA
  #define RTC_SLOW_DATA
#endif

// ==== Memory Access Hints ====
// ESP32 handles unaligned access in hardware - remove Doom's alignment code
#ifdef ESP32
  #define UNALIGNED_OK 1
  #define NO_ALIGNMENT_CHECK
#endif

// ==== ESP32 Instruction Set Hacks ====
#ifdef ESP32

// Xtensa dual-16-bit multiply (2x faster than scalar)
static FORCE_INLINE int32_t xtensa_fixedmul_dual(int16_t a, int16_t b) {
    int32_t result;
    __asm__ volatile (
        "mull.dd.ll %0, %1, %2"
        : "=a"(result) 
        : "a"(a), "a"(b)
    );
    return result;
}

// Fast unaligned 32-bit read (no penalty on ESP32)
static FORCE_INLINE uint32_t read_u32_unaligned(const void *ptr) {
    return *((const uint32_t*)ptr);
}

// Fast unaligned 16-bit read
static FORCE_INLINE uint16_t read_u16_unaligned(const void *ptr) {
    return *((const uint16_t*)ptr);
}

#endif // ESP32

// ==== Data Migration Macros ====
// Use these to mark which data goes where

// Large tables (mobjinfo, states, etc) -> IRAM
// NOTE: NOT const - Doom modifies these at runtime (e.g., turbo mode, cheats)
#define DOOM_IRAM_TABLE IRAM_DATA

// Rarely accessed buffers (ticdata, stats) -> RTC Fast  
#define DOOM_RTC_BUFFER RTC_FAST_DATA

// Sound/music tables (accessed infrequently) -> RTC Slow
#define DOOM_RTC_LOOKUP RTC_SLOW_DATA const

// Keep in DRAM (frequently accessed, needs speed)
#define DOOM_DRAM

// ==== Compiler Hints for ESP32 ====
#ifdef ESP32
  // Suggest padding alignment for ESP32 cache lines (32 bytes)
  #define CACHE_ALIGNED __attribute__((aligned(32)))
  
  // Pack structures tightly (ESP32 handles unaligned)
  #define PACKED __attribute__((packed))
  
  // Hot path function - inline and put in IRAM
  #define HOT_PATH IRAM_FUNC __attribute__((hot))
  
  // Cold path function - can stay in flash
  #define COLD_PATH __attribute__((cold))
#else
  #define CACHE_ALIGNED
  #define PACKED
  #define HOT_PATH
  #define COLD_PATH
#endif

#endif // ESP32_MEM_HACK_H
