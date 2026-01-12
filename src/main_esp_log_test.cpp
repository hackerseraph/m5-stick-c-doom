// Test using ESP_LOGI which works from boot without Serial.begin()
#include <Arduino.h>
#include "esp_log.h"
#include "esp_partition.h"

static const char *TAG = "DOOM";

void setup() {
    ESP_LOGI(TAG, "Boot starting...");
    ESP_LOGI(TAG, "Free heap: %d bytes", ESP.getFreeHeap());
    
    // Find WAD partition
    const esp_partition_t *wad_partition = esp_partition_find_first(
        static_cast<esp_partition_type_t>(66),
        static_cast<esp_partition_subtype_t>(6),
        NULL
    );
    
    if (!wad_partition) {
        ESP_LOGE(TAG, "WAD partition not found!");
    } else {
        ESP_LOGI(TAG, "WAD partition found at: 0x%x (%d bytes)", 
                 wad_partition->address, wad_partition->size);
        
        // Try to map it
        const void *mapped_ptr = NULL;
        spi_flash_mmap_handle_t mmap_handle;
        
        ESP_LOGI(TAG, "Attempting mmap...");
        esp_err_t err = esp_partition_mmap(
            wad_partition,
            0,
            1296860,
            SPI_FLASH_MMAP_DATA,
            &mapped_ptr,
            &mmap_handle
        );
        
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "mmap failed: %d", err);
        } else {
            ESP_LOGI(TAG, "mmap SUCCESS! Mapped to: %p", mapped_ptr);
            
            // Read first 4 bytes
            const unsigned char *wad_data = (const unsigned char *)mapped_ptr;
            ESP_LOGI(TAG, "WAD signature: %c%c%c%c", 
                     wad_data[0], wad_data[1], wad_data[2], wad_data[3]);
        }
    }
    
    ESP_LOGI(TAG, "Test complete!");
}

void loop() {
    delay(5000);
    ESP_LOGI(TAG, "Still running... Heap: %d", ESP.getFreeHeap());
}
