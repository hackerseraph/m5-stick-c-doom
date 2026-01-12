// M5StickC Plus GBA DOOM - Main Entry Point
// Based on Sprite_tm's esp32c3-doom-bauble

#include <Arduino.h>
#include <M5StickCPlus.h>
#include "esp_partition.h"
#include "esp_spi_flash.h"
#include <gbadoom.h>  // This will trigger PlatformIO to include the gbadoom library

// Global WAD pointer
const unsigned char *doom_iwad = NULL;
const unsigned int doom_iwad_len = 2000000;  // 2MB max (our WAD is 1.3MB)

void setup() {
    // Initialize M5StickC Plus (disable WiFi/BT to save RAM)
    M5.begin(true, true, false);  // LCD=true, Serial=true, I2C=false
    Serial.begin(115200);
    
    Serial.println("===================================");
    Serial.println("    M5StickC Plus GBA DOOM");
    Serial.println("===================================");
    
    // Memory-map the WAD partition from flash
    Serial.println("Mapping WAD from flash...");
    const esp_partition_t *wad_partition = esp_partition_find_first(
        static_cast<esp_partition_type_t>(66),  // Type: 66 (custom)
        static_cast<esp_partition_subtype_t>(6), // SubType: 6 (custom)
        NULL
    );
    
    if (wad_partition == NULL) {
        Serial.println("ERROR: WAD partition not found!");
        Serial.println("Did you flash the WAD? Run:");
        Serial.println("  esptool.py write_flash 0x210000 doom1gba.wad");
        while(1) { delay(1000); }
    }
    
    Serial.printf("WAD partition found at 0x%x, size: %d bytes\n", 
                  wad_partition->address, wad_partition->size);
    
    // Memory-map the partition
    spi_flash_mmap_handle_t mmap_handle;
    esp_err_t err = esp_partition_mmap(
        wad_partition,
        0,                        // offset
        doom_iwad_len,           // size to map
        SPI_FLASH_MMAP_DATA,     // map as data
        (const void **)&doom_iwad,
        &mmap_handle
    );
    
    if (err != ESP_OK) {
        Serial.printf("ERROR: Failed to map WAD partition: %d\n", err);
        while(1) { delay(1000); }
    }
    
    Serial.printf("WAD mapped to address: 0x%08x\n", (uint32_t)doom_iwad);
    
    // Verify WAD header
    if (doom_iwad[0] == 'I' && doom_iwad[1] == 'W' && 
        doom_iwad[2] == 'A' && doom_iwad[3] == 'D') {
        Serial.println("WAD header verified: IWAD");
    } else {
        Serial.println("ERROR: Invalid WAD header!");
        Serial.printf("Got: %c%c%c%c\n", doom_iwad[0], doom_iwad[1], 
                      doom_iwad[2], doom_iwad[3]);
        while(1) { delay(1000); }
    }
    
    // Initialize hardware
    Serial.println("Initializing input...");
    input_init();
    
    Serial.println("Initializing display...");
    lcd_init();
    
    // Initialize DOOM
    Serial.println("Initializing DOOM engine...");
    I_Init();
    I_PreInitGraphics();
    
    Serial.println("Initializing zone memory...");
    Z_Init();
    
    Serial.println("Initializing globals...");
    InitGlobals();
    
    Serial.println("Starting DOOM...");
    Serial.println("===================================");
    
    // Start DOOM main loop (never returns)
    D_DoomMain();
}

void loop() {
    // DOOM runs in its own loop, this should never be reached
    delay(1000);
}
