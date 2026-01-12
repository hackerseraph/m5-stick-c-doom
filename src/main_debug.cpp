// M5StickC Plus GBA DOOM - Step-by-step with memory tracking
#include <Arduino.h>
#include <M5StickCPlus.h>
#include "esp_partition.h"
#include "esp_spi_flash.h"
#include <gbadoom.h>

// Global WAD pointer
const unsigned char *doom_iwad = NULL;
const unsigned int doom_iwad_len = 2000000;

void printMem(const char* label) {
    Serial.printf("[%s] Free heap: %d bytes, Largest block: %d\n", 
                  label, ESP.getFreeHeap(), ESP.getMaxAllocHeap());
}

void setup() {
    Serial.begin(115200);
    delay(1000);
    
    Serial.println("\n===================================");
    Serial.println("   M5StickC Plus GBA DOOM - DEBUG");
    Serial.println("===================================");
    printMem("START");
    
    // Step 1: M5 Init
    Serial.println("\n[1] Initializing M5StickC...");
    M5.begin(true, true, false);  // LCD=true, Serial=true, I2C=false
    printMem("After M5.begin");
    
    // Step 2: Map WAD
    Serial.println("\n[2] Mapping WAD...");
    const esp_partition_t *wad_partition = esp_partition_find_first(
        static_cast<esp_partition_type_t>(66),
        static_cast<esp_partition_subtype_t>(6),
        NULL
    );
    
    if (!wad_partition) {
        Serial.println("ERROR: WAD partition not found!");
        while(1) delay(1000);
    }
    
    spi_flash_mmap_handle_t mmap_handle;
    esp_err_t err = esp_partition_mmap(
        wad_partition, 0, doom_iwad_len, SPI_FLASH_MMAP_DATA,
        (const void **)&doom_iwad, &mmap_handle
    );
    
    if (err != ESP_OK) {
        Serial.printf("ERROR: mmap failed: %d\n", err);
        while(1) delay(1000);
    }
    
    if (doom_iwad[0]!='I' || doom_iwad[1]!='W' || doom_iwad[2]!='A' || doom_iwad[3]!='D') {
        Serial.println("ERROR: Invalid WAD!");
        while(1) delay(1000);
    }
    printMem("After WAD map");
    
    // Step 3: Input init
    Serial.println("\n[3] Initializing input...");
    input_init();
    printMem("After input_init");
    
    // Step 4: LCD init
    Serial.println("\n[4] Initializing display...");
    lcd_init();
    printMem("After lcd_init");
    
    // Step 5: I_Init
    Serial.println("\n[5] Calling I_Init...");
    I_Init();
    printMem("After I_Init");
    
    // Step 6: I_PreInitGraphics
    Serial.println("\n[6] Calling I_PreInitGraphics...");
    I_PreInitGraphics();
    printMem("After I_PreInitGraphics");
    
    // Step 7: Z_Init
    Serial.println("\n[7] Calling Z_Init...");
    Z_Init();
    printMem("After Z_Init");
    
    // Step 8: InitGlobals
    Serial.println("\n[8] Calling InitGlobals...");
    InitGlobals();
    printMem("After InitGlobals");
    
    // Step 9: D_DoomMain
    Serial.println("\n[9] Calling D_DoomMain...");
    Serial.println("===================================");
    D_DoomMain();  // This should never return
}

void loop() {
    delay(5000);
    printMem("LOOP");
}
