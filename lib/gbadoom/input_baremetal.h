#ifndef INPUT_BAREMETAL_H
#define INPUT_BAREMETAL_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void input_init(void);
uint32_t input_get_buttons(void);
int input_get_key(void);
void input_get_accel(float* ax, float* ay, float* az);

#ifdef __cplusplus
}
#endif

#endif // INPUT_BAREMETAL_H
