// Check heap after M5.begin() with GBA DOOM linked
#include <Arduino.h>
#include <M5StickCPlus.h>
#include <gbadoom.h>

void setup() {
    Serial.begin(115200);
    delay(2000);
    
    Serial.println("\n\n=== M5 + GBA DOOM HEAP TEST ===");
    Serial.printf("Before M5.begin(): %d bytes free\n", ESP.getFreeHeap());
    
    M5.begin(true, true, false);  // LCD, Serial, no I2C
    Serial.printf("After M5.begin():  %d bytes free\n", ESP.getFreeHeap());
    Serial.printf("Largest block:     %d bytes\n", ESP.getMaxAllocHeap());
    
    Serial.println("\nGlobals_t size would be ~100KB");
    Serial.println("Framebuffers need 2x65KB = 130KB");
    Serial.println("Total needed: ~230KB");
    Serial.println("===============================");
}

void loop() {
    Serial.print(".");
    delay(1000);
}
