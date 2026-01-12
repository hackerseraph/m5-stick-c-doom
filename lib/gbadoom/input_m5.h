// M5StickC Plus input header
#ifndef INPUT_M5_H
#define INPUT_M5_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

void input_init(void);
uint32_t input_get_buttons(void);
int input_map_to_doom_key(uint32_t buttons, bool pressed);

#ifdef __cplusplus
}
#endif

#endif
