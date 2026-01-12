// Absolute minimal test - bypass M5 library entirely
#include <Arduino.h>

void setup() {
    Serial.begin(115200);
    delay(2000);
    
    Serial.println("\n\n=== MINIMAL TEST ===");
    Serial.printf("Free heap: %d bytes\n", ESP.getFreeHeap());
    Serial.printf("Largest block: %d bytes\n", ESP.getMaxAllocHeap());
    Serial.println("===================");
}

void loop() {
    Serial.print(".");
    delay(1000);
}
