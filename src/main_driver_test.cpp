// Test bare-metal drivers without DOOM
#include <Arduino.h>
#include "lcd_baremetal.h"
#include "input_baremetal.h"

void setup() {
    Serial.begin(115200);
    delay(2000);
    
    Serial.println("\n\n=== BARE-METAL DRIVER TEST ===");
    Serial.printf("Free heap: %d bytes\n", ESP.getFreeHeap());
    
    Serial.println("Initializing input...");
    input_init();
    Serial.printf("After input: %d bytes free\n", ESP.getFreeHeap());
    
    Serial.println("Initializing LCD...");
    lcd_init();
    Serial.printf("After LCD: %d bytes free\n", ESP.getFreeHeap());
    
    Serial.println("=== SUCCESS ===");
}

void loop() {
    Serial.print(".");
    delay(1000);
}
