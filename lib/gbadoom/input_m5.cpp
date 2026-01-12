// M5StickC Plus button input for GBA DOOM
#include <M5StickCPlus.h>
#include "input_m5.h"

// Button state
static uint32_t button_state = 0;

// Button mapping
#define BTN_A_MASK      (1 << 0)
#define BTN_B_MASK      (1 << 1)
#define BTN_HOME_MASK   (1 << 2)

// Simple key codes (we don't need the full doomkeys.h)
#define KEY_FIRE    'z'
#define KEY_USE     'x'
#define KEY_ESCAPE  27

extern "C" {

void input_init(void) {
    // M5StickC Plus buttons initialized in M5.begin()
}

uint32_t input_get_buttons(void) {
    M5.update();
    
    uint32_t state = 0;
    
    if (M5.BtnA.isPressed()) state |= BTN_A_MASK;      // Fire / Action
    if (M5.BtnB.isPressed()) state |= BTN_B_MASK;      // Use / Open doors
    // Note: M5StickCPlus library doesn't expose BtnPWR directly
    
    // Use IMU for movement (tilt forward/back for forward/backward)
    float accX, accY, accZ;
    M5.IMU.getAccelData(&accX, &accY, &accZ);
    
    button_state = state;
    return state;
}

// Map to DOOM keys
int input_map_to_doom_key(uint32_t buttons, bool pressed) {
    if (pressed) {
        if (buttons & BTN_A_MASK) return KEY_FIRE;
        if (buttons & BTN_B_MASK) return KEY_USE;
        if (buttons & BTN_HOME_MASK) return KEY_ESCAPE;
    }
    return -1;
}

} // extern "C"
