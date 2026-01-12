// Simple test - Just verify WAD mapping works
#include <Arduino.h>
#include "esp_log.h"
#include "esp_partition.h"

static const char *TAG = "SIMPLE_TEST";

// Global WAD pointer (required by gbadoom)
const unsigned char *doom_iwad = NULL;
const unsigned int doom_iwad_len = 1296860;

void setup() {
    Serial.begin(115200);
    delay(1000);  // Give serial time to initialize
    
    ESP_LOGI(TAG, "========================================");
    ESP_LOGI(TAG, "  M5StickC Plus - Simple WAD Test");
    ESP_LOGI(TAG, "========================================");
    ESP_LOGI(TAG, "Free heap: %d bytes", ESP.getFreeHeap());
    
    // 1. Find WAD partition
    ESP_LOGI(TAG, "Looking for WAD partition...");
    const esp_partition_t *wad_partition = esp_partition_find_first(
        static_cast<esp_partition_type_t>(66),  // Type 66
        static_cast<esp_partition_subtype_t>(6), // SubType 6
        NULL
    );
    
    if (!wad_partition) {
        ESP_LOGE(TAG, "ERROR: WAD partition not found!");
        ESP_LOGE(TAG, "Run: ./flash_wad.sh /dev/ttyUSB0");
        while(1) delay(1000);
    }
    
    ESP_LOGI(TAG, "WAD partition found:");
    ESP_LOGI(TAG, "  Address: 0x%x", wad_partition->address);
    ESP_LOGI(TAG, "  Size: %d bytes", wad_partition->size);
    
    // 2. Memory-map the WAD
    ESP_LOGI(TAG, "Memory-mapping WAD...");
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
        ESP_LOGE(TAG, "ERROR: mmap failed with code %d", err);
        while(1) delay(1000);
    }
    
    ESP_LOGI(TAG, "WAD mapped successfully!");
    ESP_LOGI(TAG, "  Mapped address: 0x%08x", (uint32_t)doom_iwad);
    ESP_LOGI(TAG, "  Free heap: %d bytes", ESP.getFreeHeap());
    
    // 3. Verify WAD header
    ESP_LOGI(TAG, "Verifying WAD header...");
    if (doom_iwad[0] == 'I' && doom_iwad[1] == 'W' && 
        doom_iwad[2] == 'A' && doom_iwad[3] == 'D') {
        ESP_LOGI(TAG, "✅ Valid IWAD header found!");
        ESP_LOGI(TAG, "   Header: %c%c%c%c", 
                doom_iwad[0], doom_iwad[1], doom_iwad[2], doom_iwad[3]);
    } else {
        ESP_LOGE(TAG, "❌ Invalid WAD header!");
        ESP_LOGE(TAG, "   Got: 0x%02x 0x%02x 0x%02x 0x%02x", 
                doom_iwad[0], doom_iwad[1], doom_iwad[2], doom_iwad[3]);
        while(1) delay(1000);
    }
    
    ESP_LOGI(TAG, "========================================");
    ESP_LOGI(TAG, "✅ WAD TEST PASSED!");
    ESP_LOGI(TAG, "========================================");
    ESP_LOGI(TAG, "Ready to initialize DOOM...");
}

void loop() {
    delay(5000);
    ESP_LOGI(TAG, "Idle... Free heap: %d bytes", ESP.getFreeHeap());
}
