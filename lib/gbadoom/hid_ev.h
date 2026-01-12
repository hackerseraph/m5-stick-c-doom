// Stub for HID event (we use simple button input instead)
#ifndef HID_EV_H
#define HID_EV_H

#include <stdint.h>

// Event types
#define HIDEV_EVENT_KEY_DOWN 1
#define HIDEV_EVENT_KEY_UP 2
#define HIDEV_EVENT_MOUSE_BUTTONDOWN 3
#define HIDEV_EVENT_MOUSE_BUTTONUP 4
#define HIDEV_EVENT_MOUSE_MOTION 5
#define HIDEV_EVENT_MOUSE_WHEEL 6
#define HIDEV_EVENT_JOY_AXIS 7
#define HIDEV_EVENT_JOY_BUTTONDOWN 8
#define HIDEV_EVENT_JOY_BUTTONUP 9

// Key definitions (ASCII-like + special)
#define KEY_A 'a'
#define KEY_S 's'
#define KEY_Z 'z'
#define KEY_X 'x'
#define KEY_TAB 9
#define KEY_ENTER 13
#define KEY_UP 128
#define KEY_DOWN 129
#define KEY_LEFT 130
#define KEY_RIGHT 131

typedef struct {
    uint8_t type;
    union {
        struct {
            uint8_t keycode;
        } key;
        uint8_t no;  // button number
        struct {
            int16_t dx;
            int16_t dy;
        } mouse_motion;
        struct {
            int8_t d;
        } mouse_wheel;
        struct {
            uint8_t axis;
            int16_t pos;
        } joyaxis;
    };
    uint8_t state;
} hid_ev_t;

// Stub - we don't use event queue
static inline int input_get_event(hid_ev_t* ev) {
    return 0; // No events
}

#endif
