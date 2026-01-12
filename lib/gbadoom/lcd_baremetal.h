#ifndef LCD_BAREMETAL_H
#define LCD_BAREMETAL_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void lcd_init(void);
uint16_t* lcd_get_framebuffer(void);
void lcd_wait_for_vsync(void);
void lcd_flip(void);
void lcd_set_pal(uint8_t *pal);
void lcd_render_fb(const uint8_t *fb);

#ifdef __cplusplus
}
#endif

#endif // LCD_BAREMETAL_H
