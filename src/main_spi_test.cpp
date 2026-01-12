// Minimal test - Arduino + bare SPI only
#include <Arduino.h>
#include <SPI.h>

void setup() {
    Serial.begin(115200);
    delay(2000);
    
    Serial.println("\n=== MINIMAL SPI TEST ===");
    Serial.printf("Free heap: %d bytes\n", ESP.getFreeHeap());
    
    // Just init SPI
    SPIClass spi(VSPI);
    spi.begin(13, -1, 15, 5);  // SCK, MISO, MOSI, CS
    Serial.println("SPI initialized");
    Serial.printf("Free heap: %d bytes\n", ESP.getFreeHeap());
    
    Serial.println("SUCCESS!");
}

void loop() {
    Serial.print(".");
    delay(1000);
}
