// Manually replicate Sprite_tm's initialization WITHOUT calling D_DoomMain
// This avoids linking in D_DoomLoop and other code that might have problematic static data
#include <Arduino.h>
#include "esp_log.h"
#include "esp_partition.h"

// Include DOOM headers but DON'T call D_DoomMain
#include <gbadoom.h>

static const char *TAG = "MANUAL_INIT";

// Global WAD pointer  
const unsigned char *doom_iwad = NULL;
const unsigned int doom_iwad_len = 1296860;

void setup() {
    ESP_LOGI(TAG, "===================================");
    ESP_LOGI(TAG, "Manual DOOM Init (Sprite_tm order)");
    ESP_LOGI(TAG, "===================================");
    ESP_LOGI(TAG, "Free heap: %d bytes", ESP.getFreeHeap());
    
    // 1. Map WAD
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
    
    // 2. Input init
    ESP_LOGI(TAG, "Calling input_init()...");
    input_init();
    ESP_LOGI(TAG, "input_init() OK. Heap: %d", ESP.getFreeHeap());
    
    // 3. I_PreInitGraphics (this calls lcd_init internally and allocates framebuffer)
    ESP_LOGI(TAG, "Calling I_PreInitGraphics()...");
    I_PreInitGraphics();
    ESP_LOGI(TAG, "I_PreInitGraphics() OK. Heap: %d", ESP.getFreeHeap());
    
    // Skip PrintVer since it's static
    
    // 4. Z_Init
    ESP_LOGI(TAG, "Calling Z_Init()...");
    Z_Init();
    ESP_LOGI(TAG, "Z_Init() OK. Heap: %d", ESP.getFreeHeap());
    
    // 5. InitGlobals
    ESP_LOGI(TAG, "Calling InitGlobals()...");
    InitGlobals();
    ESP_LOGI(TAG, "InitGlobals() OK. Heap: %d", ESP.getFreeHeap());
    
    // Now the KEY TEST - call D_DoomMain which should call D_DoomMainSetup then D_DoomLoop
    ESP_LOGI(TAG, "NOW CALLING D_DoomMain() - this is where Sprite_tm succeeds but we crash!");
    D_DoomMain();  // This never returns if successful
    
    ESP_LOGI(TAG, "=== This should never print ===");
}

void loop() {
    delay(5000);
    ESP_LOGI(TAG, "Still running... Heap: %d", ESP.getFreeHeap());
}
