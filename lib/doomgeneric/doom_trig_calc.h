// Real-time trig calculation to replace lookup tables
// 240MHz ESP32 can calculate sin/cos MUCH faster than reading from slow RAM
// Uses fast approximation algorithms optimized for game use

#ifndef DOOM_TRIG_CALC_H
#define DOOM_TRIG_CALC_H

#include "m_fixed.h"
#include <math.h>

// Fast sine using ESP32's hardware FPU
// ESP32 has single-cycle FPU operations - this is FAST
static inline fixed_t doom_calc_sin(int angle) {
    // Normalize angle to 0-8192 range (FINEANGLES)
    angle = angle & 8191;
    
    // Convert to radians: angle * 2*PI / 8192
    float rad = ((float)angle * 0.000767398163397448f); // 2*PI/8192 pre-calculated
    
    // ESP32 FPU sinf is extremely fast
    return (fixed_t)(sinf(rad) * 65536.0f);
}

static inline fixed_t doom_calc_cos(int angle) {
    // cos(x) = sin(x + π/2)
    return doom_calc_sin(angle + 2048); // Add 90 degrees (8192/4)
}

static inline fixed_t doom_calc_tan(int angle) {
    angle = angle & 8191;
    float rad = ((float)angle * 0.000767398163397448f);
    return (fixed_t)(tanf(rad) * 65536.0f);
}

// Ultra-fast approximate inverse tangent for angle lookups
// Uses polynomial approximation
static inline unsigned fast_atan2_angle(int y, int x) {
    if (x == 0) {
        return (y > 0) ? 0x40000000 : 0xC0000000; // ±90°
    }
    
    float fx = (float)x;
    float fy = (float)y;
    float ratio = fy / fx;
    
    // Fast atan approximation using polynomial
    float atan_val = ratio / (1.0f + 0.28f * ratio * ratio);
    
    // Convert to BAM (Binary Angle Measurement)
    unsigned angle = (unsigned)(atan_val * 683565275.5f); // 2^32 / (2*PI)
    
    // Adjust for quadrant
    if (x < 0) angle += 0x80000000;
    if (y < 0 && x > 0) angle = -angle;
    
    return angle;
}

// Macro to replace table lookups - compiler will inline these
#define FINETANGENT(angle) fast_tan_fixed(angle)
#define FINESINE(angle) fast_sin_fixed(angle)
#define FINECOSINE(angle) fast_cos_fixed(angle)

#endif // DOOM_TRIG_CALC_H
