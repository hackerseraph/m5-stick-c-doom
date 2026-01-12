// M5StickC Plus GBA DOOM - Bare-metal version (no M5 library)
#include <Arduino.h>
#include "esp_partition.h"
#include "esp_spi_flash.h"
#include "esp_log.h"
#include <gbadoom.h>

static const char *TAG = "DOOM";

// Global WAD pointer
const unsigned char *doom_iwad = NULL;
const unsigned int doom_iwad_len = 1296860;  // Actual doom1gba.wad size

void setup() {
    ESP_LOGI(TAG, "\n\n===================================");
    ESP_LOGI(TAG, "  M5StickC Plus GBA DOOM (Bare-metal)");
    ESP_LOGI(TAG, "===================================");
    ESP_LOGI(TAG, "Free heap: %d bytes", ESP.getFreeHeap());
    
    // Memory-map the WAD partition from flash
    ESP_LOGI(TAG, "Mapping WAD from flash...");
    const esp_partition_t *wad_partition = esp_partition_find_first(
        static_cast<esp_partition_type_t>(66),
        static_cast<esp_partition_subtype_t>(6),
        NULL
    );
    
    if (wad_partition == NULL) {
        ESP_LOGE(TAG, "ERROR: WAD partition not found!");
        while(1) { delay(1000); }
    }
    
    ESP_LOGI(TAG, "WAD partition: 0x%x (%d bytes)", 
                  wad_partition->address, wad_partition->size);
    
    // Memory-map the partition
    spi_flash_mmap_handle_t mmap_handle;
    esp_err_t err = esp_partition_mmap(
        wad_partition,
        0,
        doom_iwad_len,
        SPI_FLASH_MMAP_DATA,
        (const void **)&doom_iwad,
        &mmap_handle
    );
    
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "ERROR: Failed to map WAD: %d", err);
        while(1) { delay(1000); }
    }
    
    ESP_LOGI(TAG, "WAD mapped to: 0x%08x", (uint32_t)doom_iwad);
    
    // Verify WAD header
    if (doom_iwad[0] == 'I' && doom_iwad[1] == 'W' && 
        doom_iwad[2] == 'A' && doom_iwad[3] == 'D') {
        ESP_LOGI(TAG, "WAD verified: IWAD");
    } else {
        ESP_LOGE(TAG, "ERROR: Invalid WAD header: %c%c%c%c", 
                      doom_iwad[0], doom_iwad[1], doom_iwad[2], doom_iwad[3]);
        while(1) { delay(1000); }
    }
    
    ESP_LOGI(TAG, "Free heap before init: %d bytes", ESP.getFreeHeap());
    
    // Initialize hardware (bare-metal drivers)
    ESP_LOGI(TAG, "Initializing input...");
    input_init();
    ESP_LOGI(TAG, "Free heap after input: %d bytes", ESP.getFreeHeap());
    
    ESP_LOGI(TAG, "Initializing display...");
    lcd_init();
    ESP_LOGI(TAG, "Free heap after LCD: %d bytes", ESP.getFreeHeap());
    
    // Initialize DOOM
    ESP_LOGI(TAG, "Initializing DOOM engine...");
    I_Init();
    ESP_LOGI(TAG, "Free heap after I_Init: %d bytes", ESP.getFreeHeap());
    
    I_PreInitGraphics();
    ESP_LOGI(TAG, "Free heap after graphics: %d bytes", ESP.getFreeHeap());
    
    ESP_LOGI(TAG, "Initializing zone memory...");
    Z_Init();
    ESP_LOGI(TAG, "Free heap after Z_Init: %d bytes", ESP.getFreeHeap());
    
    ESP_LOGI(TAG, "Initializing globals...");
    InitGlobals();
    ESP_LOGI(TAG, "Free heap after globals: %d bytes", ESP.getFreeHeap());
    
    ESP_LOGI(TAG, "Starting DOOM...");
    ESP_LOGI(TAG, "===================================");
    
    // Start DOOM main loop (never returns)
    D_DoomMain();
}

void loop() {
    // Never reached
    delay(1000);
}
