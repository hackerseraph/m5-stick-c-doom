// Bare-metal ST7789 LCD driver for M5StickC Plus
// 135x240 display, rotated to 240x135 landscape
#include <Arduino.h>
#include <SPI.h>
#include "lcd_baremetal.h"

// M5StickC Plus LCD pins
#define TFT_MOSI    15
#define TFT_SCLK    13
#define TFT_CS      5
#define TFT_DC      23
#define TFT_RST     18
#define TFT_BL      -1  // No backlight control on M5StickC Plus

// ST7789 commands
#define ST7789_NOP      0x00
#define ST7789_SWRESET  0x01
#define ST7789_SLPOUT   0x11
#define ST7789_NORON    0x13
#define ST7789_INVON    0x21
#define ST7789_DISPOFF  0x28
#define ST7789_DISPON   0x29
#define ST7789_CASET    0x2A
#define ST7789_RASET    0x2B
#define ST7789_RAMWR    0x2C
#define ST7789_MADCTL   0x36
#define ST7789_COLMOD   0x3A

static SPIClass* spi = NULL;
static uint16_t* framebuffer = NULL;
static uint16_t palette[256];

static void lcd_cmd(uint8_t cmd) {
    digitalWrite(TFT_DC, LOW);
    digitalWrite(TFT_CS, LOW);
    spi->transfer(cmd);
    digitalWrite(TFT_CS, HIGH);
}

static void lcd_data(uint8_t data) {
    digitalWrite(TFT_DC, HIGH);
    digitalWrite(TFT_CS, LOW);
    spi->transfer(data);
    digitalWrite(TFT_CS, HIGH);
}

static void lcd_data16(uint16_t data) {
    digitalWrite(TFT_DC, HIGH);
    digitalWrite(TFT_CS, LOW);
    spi->transfer(data >> 8);
    spi->transfer(data & 0xFF);
    digitalWrite(TFT_CS, HIGH);
}

extern "C" {

void lcd_init(void) {
    // Initialize SPI
    spi = new SPIClass(VSPI);
    spi->begin(TFT_SCLK, -1, TFT_MOSI, TFT_CS);
    spi->setFrequency(40000000);  // 40MHz
    
    pinMode(TFT_CS, OUTPUT);
    pinMode(TFT_DC, OUTPUT);
    pinMode(TFT_RST, OUTPUT);
    
    digitalWrite(TFT_CS, HIGH);
    digitalWrite(TFT_DC, HIGH);
    
    // Hardware reset
    digitalWrite(TFT_RST, LOW);
    delay(10);
    digitalWrite(TFT_RST, HIGH);
    delay(120);
    
    // Initialize ST7789
    lcd_cmd(ST7789_SWRESET);
    delay(150);
    
    lcd_cmd(ST7789_SLPOUT);
    delay(120);
    
    lcd_cmd(ST7789_COLMOD);  // 16-bit color
    lcd_data(0x55);
    
    lcd_cmd(ST7789_MADCTL);  // Rotation: landscape 240x135
    lcd_data(0x60);  // MY=0, MX=1, MV=1, ML=0, RGB=1
    
    lcd_cmd(ST7789_INVON);   // Inversion on
    delay(10);
    
    lcd_cmd(ST7789_NORON);   // Normal display on
    delay(10);
    
    lcd_cmd(ST7789_DISPON);  // Display on
    delay(120);
    
    // Allocate framebuffer (240x135 RGB565 = 64.8KB)
    // Don't use DMA memory - regular heap is fine
    framebuffer = (uint16_t*)malloc(240 * 135 * 2);
    if (!framebuffer) {
        Serial.println("ERROR: Failed to allocate framebuffer!");
        Serial.printf("Free heap: %d bytes\n", ESP.getFreeHeap());
        while(1);
    }
    
    memset(framebuffer, 0, 240 * 135 * 2);
    Serial.println("LCD initialized: 240x135");
}

uint16_t* lcd_get_framebuffer(void) {
    return framebuffer;
}

void lcd_wait_for_vsync(void) {
    // No vsync on ST7789, just delay
    delayMicroseconds(100);
}

void lcd_flip(void) {
    if (!framebuffer) return;
    
    // Set window to full screen
    lcd_cmd(ST7789_CASET);
    lcd_data16(52);        // X start (52 offset for 135px width on 240px chip)
    lcd_data16(52 + 239);  // X end
    
    lcd_cmd(ST7789_RASET);
    lcd_data16(40);        // Y start (40 offset for 135px height)
    lcd_data16(40 + 134);  // Y end
    
    lcd_cmd(ST7789_RAMWR);
    
    // Bulk transfer framebuffer
    digitalWrite(TFT_DC, HIGH);
    digitalWrite(TFT_CS, LOW);
    
    spi->writeBytes((uint8_t*)framebuffer, 240 * 135 * 2);
    
    digitalWrite(TFT_CS, HIGH);
}

void lcd_set_pal(uint8_t *pal) {
    // Convert 8-bit RGB palette to RGB565
    for (int i = 0; i < 256; i++) {
        uint8_t r = pal[i * 3 + 0];
        uint8_t g = pal[i * 3 + 1];
        uint8_t b = pal[i * 3 + 2];
        palette[i] = ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
    }
}

void lcd_render_fb(const uint8_t *fb) {
    if (!framebuffer) return;
    
    // Convert 8-bit indexed to RGB565
    for (int i = 0; i < 240 * 135; i++) {
        framebuffer[i] = palette[fb[i]];
    }
}

} // extern "C"
