// Test WAD partition mapping only
#include <Arduino.h>
#include "esp_partition.h"
#include "esp_system.h"

void setup() {
    Serial.begin(115200);
    delay(100);  // Brief delay for Serial stability
    
    Serial.println("\n=== WAD MMAP TEST ===");
    Serial.printf("Free heap: %d bytes\n", ESP.getFreeHeap());
    
    // Find WAD partition
    const esp_partition_t *wad_partition = esp_partition_find_first(
        static_cast<esp_partition_type_t>(66),
        static_cast<esp_partition_subtype_t>(6),
        NULL
    );
    
    if (!wad_partition) {
        Serial.println("ERROR: WAD partition not found!");
        while(1) delay(1000);
    }
    
    Serial.printf("WAD partition found at: 0x%x (%d bytes)\n", 
                  wad_partition->address, wad_partition->size);
    Serial.printf("Free heap: %d bytes\n", ESP.getFreeHeap());
    
    // Try to map the WAD
    const void *mapped_ptr = NULL;
    spi_flash_mmap_handle_t mmap_handle;
    
    Serial.println("Attempting to mmap WAD...");
    esp_err_t err = esp_partition_mmap(
        wad_partition,
        0,
        1296860,  // Actual WAD size
        SPI_FLASH_MMAP_DATA,
        &mapped_ptr,
        &mmap_handle
    );
    
    if (err != ESP_OK) {
        Serial.printf("ERROR: mmap failed with error %d\n", err);
        Serial.printf("Free heap: %d bytes\n", ESP.getFreeHeap());
        while(1) delay(1000);
    }
    
    Serial.printf("SUCCESS! WAD mapped to: %p\n", mapped_ptr);
    Serial.printf("Free heap: %d bytes\n", ESP.getFreeHeap());
    
    // Read first 4 bytes to verify it's a WAD file
    const unsigned char *wad_data = (const unsigned char *)mapped_ptr;
    Serial.printf("WAD signature: %c%c%c%c\n", 
                  wad_data[0], wad_data[1], wad_data[2], wad_data[3]);
    
    Serial.println("\nMMAP TEST PASSED!");
}

void loop() {
    delay(1000);
    Serial.println("Still running...");
}
