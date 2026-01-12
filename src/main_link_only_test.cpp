// Test which DOOM function causes crash
#include <Arduino.h>
#include "esp_log.h"
#include "esp_partition.h"

// Include DOOM header
#include <gbadoom.h>

static const char *TAG = "DOOM_FUNC_TEST";

// Global WAD pointer  
const unsigned char *doom_iwad = NULL;
const unsigned int doom_iwad_len = 1296860;

void setup() {
    ESP_LOGI(TAG, "===================================");
    ESP_LOGI(TAG, "DOOM Function Test");
    ESP_LOGI(TAG, "===================================");
    ESP_LOGI(TAG, "Free heap: %d bytes", ESP.getFreeHeap());
    
    // Map WAD first
    ESP_LOGI(TAG, "Mapping WAD...");
    const esp_partition_t *wad_partition = esp_partition_find_first(
        static_cast<esp_partition_type_t>(66),
        static_cast<esp_partition_subtype_t>(6),
        NULL
    );
    
    if (!wad_partition) {
        ESP_LOGE(TAG, "WAD partition not found!");
        while(1) delay(1000);
    }
    
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
        ESP_LOGE(TAG, "mmap failed: %d", err);
        while(1) delay(1000);
    }
    
    ESP_LOGI(TAG, "WAD mapped OK. Heap: %d", ESP.getFreeHeap());
    
    // Test bare-metal drivers
    ESP_LOGI(TAG, "Calling input_init()...");
    input_init();
    ESP_LOGI(TAG, "input_init() OK. Heap: %d", ESP.getFreeHeap());
    
    ESP_LOGI(TAG, "Calling lcd_init()...");
    lcd_init();
    ESP_LOGI(TAG, "lcd_init() OK. Heap: %d", ESP.getFreeHeap());
    
    // Test I_Init()
    ESP_LOGI(TAG, "Calling I_Init()...");
    I_Init();
    ESP_LOGI(TAG, "I_Init() OK. Heap: %d", ESP.getFreeHeap());
    
    ESP_LOGI(TAG, "Calling I_PreInitGraphics()...");
    I_PreInitGraphics();
    ESP_LOGI(TAG, "I_PreInitGraphics() OK. Heap: %d", ESP.getFreeHeap());
    
    ESP_LOGI(TAG, "Calling Z_Init()...");
    Z_Init();
    ESP_LOGI(TAG, "Z_Init() OK. Heap: %d", ESP.getFreeHeap());
    
    ESP_LOGI(TAG, "Calling InitGlobals()...");
    InitGlobals();
    ESP_LOGI(TAG, "InitGlobals() OK. Heap: %d", ESP.getFreeHeap());
    
    ESP_LOGI(TAG, "Calling D_DoomMainSetup() manually (not D_DoomMain)");
    // D_DoomMainSetup();  // Try calling the setup function directly
    ESP_LOGI(TAG, "Skipping D_DoomMainSetup for now");
    
    ESP_LOGI(TAG, "===  TEST COMPLETE - will loop ===");
}

void loop() {
    delay(5000);
    ESP_LOGI(TAG, "Still running... Heap: %d", ESP.getFreeHeap());
}
