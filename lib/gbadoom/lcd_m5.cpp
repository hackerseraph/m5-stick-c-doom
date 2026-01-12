// M5StickC Plus LCD driver for GBA DOOM
// ST7789 135x240 display driver

#include <M5StickCPlus.h>
#include "lcd_m5.h"

// Framebuffer for DOOM rendering  
static uint16_t *framebuffer = NULL;
static uint16_t palette[256];  // 8-bit palette for GBA DOOM

extern "C" {

void lcd_init(void) {
    M5.Lcd.begin();
    M5.Lcd.setRotation(3);  // Landscape: 240x135
    M5.Lcd.fillScreen(TFT_BLACK);
    
    // Allocate framebuffer (240x135 = 32,400 pixels * 2 bytes = 64.8KB)
    framebuffer = (uint16_t *)malloc(240 * 135 * sizeof(uint16_t));
    if (!framebuffer) {
        Serial.println("Failed to allocate framebuffer!");
    } else {
        Serial.println("LCD initialized: 240x135");
    }
}

uint16_t* lcd_get_framebuffer(void) {
    return framebuffer;
}

void lcd_wait_for_vsync(void) {
    // M5StickC Plus has no vsync signal, just delay slightly
    vTaskDelay(1);
}

void lcd_flip(void) {
    if (framebuffer) {
        // Push framebuffer to LCD
        M5.Lcd.pushImage(0, 0, 240, 135, framebuffer);
    }
}

void lcd_set_pal(uint8_t *pal) {
    // Convert 8-bit RGB palette to RGB565
    for (int i = 0; i < 256; i++) {
        uint8_t r = pal[i * 3 + 0];
        uint8_t g = pal[i * 3 + 1];
        uint8_t b = pal[i * 3 + 2];
        // Convert to RGB565
        palette[i] = ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
    }
}

void lcd_render_fb(const uint8_t *fb) {
    // Convert 8-bit indexed framebuffer to RGB565 using palette
    if (!framebuffer) return;
    
    for (int i = 0; i < 240 * 135; i++) {
        framebuffer[i] = palette[fb[i]];
    }
    
    // Display the framebuffer
    lcd_flip();
}

void lcd_deinit(void) {
    if (framebuffer) {
        free(framebuffer);
        framebuffer = NULL;
    }
}

} // extern "C"
