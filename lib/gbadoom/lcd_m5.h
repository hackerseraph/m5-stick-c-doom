// M5StickC Plus LCD driver header
#ifndef LCD_M5_H
#define LCD_M5_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void lcd_init(void);
uint16_t* lcd_get_framebuffer(void);
void lcd_wait_for_vsync(void);
void lcd_flip(void);
void lcd_set_pal(uint8_t *pal);         // Set 8-bit palette
void lcd_render_fb(const uint8_t *fb);  // Render indexed framebuffer
void lcd_deinit(void);

#ifdef __cplusplus
}
#endif

#endif
