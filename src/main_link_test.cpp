// Test if just linking GBA DOOM causes crash
#include <Arduino.h>
#include <gbadoom.h>  // This will link the GBA DOOM library

void setup() {
    Serial.begin(115200);
    delay(2000);
    
    Serial.println("\n\n=== GBA DOOM LINK TEST ===");
    Serial.printf("Free heap: %d bytes\n", ESP.getFreeHeap());
    Serial.println("GBA DOOM library linked but not initialized");
    Serial.println("==========================");
}

void loop() {
    Serial.print(".");
    delay(1000);
}
