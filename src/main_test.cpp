// Minimal test to see if M5 boots
#include <Arduino.h>
#include <M5StickCPlus.h>

void setup() {
    Serial.begin(115200);
    delay(1000);
    Serial.println("\n\n=== TEST START ===");
    
    Serial.println("About to call M5.begin()...");
    M5.begin();
    Serial.println("M5.begin() completed!");
    
    M5.Lcd.fillScreen(TFT_RED);
    M5.Lcd.setCursor(0, 0);
    M5.Lcd.setTextColor(TFT_WHITE);
    M5.Lcd.setTextSize(2);
    M5.Lcd.println("HELLO");
    
    Serial.println("Display test completed!");
}

void loop() {
    Serial.println("Loop running...");
    delay(1000);
}
