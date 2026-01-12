// doomgeneric port for M5StickC Plus 1.1
// This is a proper implementation of actual Doom using the doomgeneric framework
// Optimized for 320KB RAM without PSRAM using advanced memory techniques

#include <M5StickCPlus.h>
#include <LittleFS.h>
extern "C" {
#include "../lib/doomgeneric/doomgeneric.h"
#include "../lib/doomgeneric/doomkeys.h"
}

// Memory optimization: Use 8-bit indexed color mode
#define CMAP256 1

// Display configuration - optimized for RAM constraints
#define SCREEN_WIDTH 135
#define SCREEN_HEIGHT 240
#define DOOMGENERIC_RESX 60
#define DOOMGENERIC_RESY 108

// NOTE: Framebuffer is allocated by doomgeneric_Create() in doomgeneric.c
// We don't declare it here to avoid duplication

// Color palette - convert Doom's palette to RGB565 (heap allocated to save BSS)
static uint16_t* doom_palette = NULL;

// Key queue for input handling
#define KEY_QUEUE_SIZE 16
static struct {
    unsigned char keys[KEY_QUEUE_SIZE];
    int pressed[KEY_QUEUE_SIZE];
    int write_idx;
    int read_idx;
} key_queue;

// Timing
static uint32_t start_time_ms;

// Player state for controls
static float player_turn_speed = 0.0f;

// Initialize hardware
extern "C" void DG_Init() {
    M5.begin();
    M5.Lcd.setRotation(0);  // Portrait mode
    M5.Lcd.fillScreen(BLACK);
    M5.Axp.ScreenBreath(12);  // Max brightness
    
    // Initialize timing
    start_time_ms = millis();
    
    // Initialize key queue
    key_queue.write_idx = 0;
    key_queue.read_idx = 0;
    
    // Allocate palette on heap
    if (!doom_palette) {
        doom_palette = (uint16_t*)malloc(256 * sizeof(uint16_t));
    }
    
    // Initialize default Doom palette (we'll update this from WAD later)
    // For now, create a grayscale palette
    for (int i = 0; i < 256; i++) {
        uint8_t val = i;
        doom_palette[i] = M5.Lcd.color565(val, val, val);
    }
    
    Serial.begin(115200);
    Serial.println("Doom M5StickC Plus - Initialized");
    Serial.printf("Resolution: %dx%d\n", DOOMGENERIC_RESX, DOOMGENERIC_RESY);
    Serial.printf("Framebuffer size: %d bytes\n", DOOMGENERIC_RESX * DOOMGENERIC_RESY);
}

// Line buffer allocated on heap to save BSS
static uint16_t* rgb565_line = NULL;

// Draw frame to LCD
extern "C" void DG_DrawFrame() {
    // Allocate line buffer on first call
    if (!rgb565_line) {
        rgb565_line = (uint16_t*)malloc(DOOMGENERIC_RESX * sizeof(uint16_t));
    }
    
    // Convert indexed color framebuffer to RGB565 line-by-line
    for (int y = 0; y < DOOMGENERIC_RESY; y++) {
        int src_offset = y * DOOMGENERIC_RESX;
        
        // Convert this line to RGB565
        for (int x = 0; x < DOOMGENERIC_RESX; x++) {
            rgb565_line[x] = doom_palette[DG_ScreenBuffer[src_offset + x]];
        }
        
        // Push line to top-left corner
        M5.Lcd.pushImage(0, y, DOOMGENERIC_RESX, 1, rgb565_line);
    }
}

// Sleep for specified milliseconds
extern "C" void DG_SleepMs(uint32_t ms) {
    delay(ms);
}

// Get milliseconds since init
extern "C" uint32_t DG_GetTicksMs() {
    return millis() - start_time_ms;
}

// Add key to queue
static void add_key_to_queue(unsigned char key, int pressed) {
    key_queue.keys[key_queue.write_idx] = key;
    key_queue.pressed[key_queue.write_idx] = pressed;
    key_queue.write_idx = (key_queue.write_idx + 1) % KEY_QUEUE_SIZE;
}

// Handle input from M5StickC Plus controls
static void handle_input() {
    M5.update();
    
    // Button A - Fire/Shoot
    static bool btnA_was_pressed = false;
    if (M5.BtnA.wasPressed() && !btnA_was_pressed) {
        add_key_to_queue(KEY_FIRE, 1);
        btnA_was_pressed = true;
    }
    if (M5.BtnA.wasReleased() && btnA_was_pressed) {
        add_key_to_queue(KEY_FIRE, 0);
        btnA_was_pressed = false;
    }
    
    // Button B - Use/Open doors
    static bool btnB_was_pressed = false;
    if (M5.BtnB.wasPressed() && !btnB_was_pressed) {
        add_key_to_queue(KEY_USE, 1);
        btnB_was_pressed = true;
    }
    if (M5.BtnB.wasReleased() && btnB_was_pressed) {
        add_key_to_queue(KEY_USE, 0);
        btnB_was_pressed = false;
    }
    
    // IMU - Tilt for turning and forward/backward movement
    float accX, accY, accZ;
    float gyroX, gyroY, gyroZ;
    M5.IMU.getAccelData(&accX, &accY, &accZ);
    M5.IMU.getGyroData(&gyroX, &gyroY, &gyroZ);
    
    // Tilt left/right to turn (use accelerometer Y axis)
    const float turn_threshold = 0.15f;
    if (accY > turn_threshold) {
        add_key_to_queue(KEY_LEFTARROW, 1);
    } else if (accY < -turn_threshold) {
        add_key_to_queue(KEY_RIGHTARROW, 1);
    }
    
    // Tilt forward/backward to move (use accelerometer X axis)
    const float move_threshold = 0.2f;
    if (accX > move_threshold) {
        add_key_to_queue(KEY_UPARROW, 1);
    } else if (accX < -move_threshold) {
        add_key_to_queue(KEY_DOWNARROW, 1);
    }
}

// Get key from queue
extern "C" int DG_GetKey(int* pressed, unsigned char* key) {
    // Handle input first
    handle_input();
    
    // Check if queue has keys
    if (key_queue.read_idx == key_queue.write_idx) {
        return 0;  // Queue empty
    }
    
    // Read from queue
    *key = key_queue.keys[key_queue.read_idx];
    *pressed = key_queue.pressed[key_queue.read_idx];
    key_queue.read_idx = (key_queue.read_idx + 1) % KEY_QUEUE_SIZE;
    
    return 1;  // Key available
}

// Set window title (not used on M5StickC)
extern "C" void DG_SetWindowTitle(const char* title) {
    Serial.printf("Doom: %s\n", title);
}

// Pointer to screen buffer for Doom engine - declare extern, defined in doomgeneric.c
extern "C" pixel_t* DG_ScreenBuffer;

// Arduino setup
void setup() {
    Serial.begin(115200);
    delay(500);
    Serial.println("\n\n=== DOOM M5StickC Plus ===");
    Serial.println("Step 1: Initializing M5...");
    Serial.flush();
    
    M5.begin();
    Serial.println("Step 2: M5 initialized!");
    Serial.flush();
    
    M5.Lcd.setRotation(0);
    M5.Lcd.fillScreen(BLACK);
    M5.Lcd.setTextColor(WHITE, BLACK);
    M5.Lcd.setTextSize(1);
    M5.Lcd.setCursor(5, 20);
    M5.Lcd.println("DOOM");
    M5.Lcd.setCursor(5, 35);
    M5.Lcd.println("Mounting LittleFS...");
    
    Serial.println("Step 3: Mounting LittleFS...");
    Serial.flush();
    
    if (!LittleFS.begin(false)) {
        Serial.println("LittleFS Mount Failed!");
        M5.Lcd.setCursor(5, 50);
        M5.Lcd.println("FS FAILED!");
        while(1) delay(1000);
    }
    
    Serial.println("Step 4: LittleFS mounted OK");
    
    // List files
    File root = LittleFS.open("/");
    File file = root.openNextFile();
    Serial.println("Files in LittleFS:");
    while(file) {
        Serial.printf("  %s (%d bytes)\n", file.name(), file.size());
        file = root.openNextFile();
    }
    Serial.flush();
    
    M5.Lcd.setCursor(5, 50);
    M5.Lcd.println("FS OK!");
    
    M5.Lcd.setCursor(5, 65);
    M5.Lcd.println("Starting...");
    
    Serial.printf("Step 5: Starting DOOM at %dx%d\n", DOOMGENERIC_RESX, DOOMGENERIC_RESY);
    Serial.flush();
    delay(2000);
    
    // Create Doom instance with IWAD from LittleFS
    char* argv[] = {
        (char*)"doom",
        (char*)"-iwad", (char*)"/doom1.wad",
        (char*)"-skill", (char*)"1",  // Easy difficulty
        (char*)"-nomusic",
        (char*)"-nosound"
    };
    int argc = 7;
    
    doomgeneric_Create(argc, argv);
}

// Arduino main loop
void loop() {
    doomgeneric_Tick();
}
