// Diagnostic version to check memory allocation
#include <Arduino.h>
#include <M5StickCPlus.h>
#include "esp_partition.h"
#include "esp_spi_flash.h"
#include <gbadoom.h>

// Global WAD pointer
const unsigned char *doom_iwad = NULL;
const unsigned int doom_iwad_len = 2000000;

void printHeap(const char* label) {
    Serial.printf("[%s] Free heap: %d bytes (largest block: %d)\n", 
                  label, ESP.getFreeHeap(), ESP.getMaxAllocHeap());
}

void setup() {
    M5.begin();
    Serial.begin(115200);
    delay(1000);
    
    Serial.println("\n\n===================================");
    Serial.println("  M5StickC Plus GBA DOOM - Memory Test");
    Serial.println("===================================");
    
    printHeap("After M5.begin()");
    
    // Map WAD
    Serial.println("Mapping WAD from flash...");
    const esp_partition_t *wad_partition = esp_partition_find_first(
        static_cast<esp_partition_type_t>(66),
        static_cast<esp_partition_subtype_t>(6),
        NULL
    );
    
    if (wad_partition == NULL) {
        Serial.println("ERROR: WAD partition not found!");
        while(1) { delay(1000); }
    }
    
    printHeap("After partition find");
    
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
        Serial.printf("ERROR: Failed to map WAD: %d\n", err);
        while(1) { delay(1000); }
    }
    
    printHeap("After WAD mmap");
    
    Serial.println("Initializing input...");
    input_init();
    printHeap("After input_init");
    
    Serial.println("Initializing LCD...");
    lcd_init();
    printHeap("After lcd_init");
    
    Serial.println("Calling I_Init()...");
    I_Init();
    printHeap("After I_Init");
    
    Serial.println("Calling I_PreInitGraphics()...");
    I_PreInitGraphics();
    printHeap("After I_PreInitGraphics");
    
    Serial.println("Calling Z_Init()...");
    Z_Init();
    printHeap("After Z_Init");
    
    Serial.println("Calling InitGlobals()...");
    InitGlobals();
    printHeap("After InitGlobals");
    
    Serial.println("\n=== All init complete! ===");
    Serial.println("Would now call D_DoomMain()...");
    
    // Don't actually call D_DoomMain, just keep running
}

void loop() {
    delay(1000);
    Serial.print(".");
}
