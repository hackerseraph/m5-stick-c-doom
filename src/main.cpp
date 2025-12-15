// doomgeneric port for M5StickC Plus 1.1
// This is a proper implementation of actual Doom using the doomgeneric framework
// Optimized for 320KB RAM without PSRAM using advanced memory techniques

#include <M5StickCPlus.h>
extern "C" {
#include "../lib/doomgeneric/doomgeneric.h"
#include "../lib/doomgeneric/doomkeys.h"
}

// Memory optimization: Use 8-bit indexed color mode
#define CMAP256 1

// Display configuration - native screen resolution
#define SCREEN_WIDTH 135
#define SCREEN_HEIGHT 240
#define DOOMGENERIC_RESX 135
#define DOOMGENERIC_RESY 240

// NOTE: Framebuffer is allocated by doomgeneric_Create() in doomgeneric.c
// We don't declare it here to avoid duplication

// Color palette - convert Doom's palette to RGB565
static uint16_t doom_palette[256];

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

// DMA buffer for faster screen updates (4KB chunks)
#define DMA_BUFFER_SIZE 4096
static uint16_t dma_buffer[DMA_BUFFER_SIZE];

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

// Draw frame to LCD
extern "C" void DG_DrawFrame() {
    // Convert indexed color framebuffer to RGB565 and send to display
    // Process in chunks to avoid huge static buffer
    
    #define CHUNK_SIZE 1350  // Process 10 rows at a time (135 * 10)
    uint16_t rgb565_chunk[CHUNK_SIZE];
    
    int pixel_count = DOOMGENERIC_RESX * DOOMGENERIC_RESY;
    int chunks = (pixel_count + CHUNK_SIZE - 1) / CHUNK_SIZE;
    
    for (int chunk = 0; chunk < chunks; chunk++) {
        int start_pixel = chunk * CHUNK_SIZE;
        int end_pixel = start_pixel + CHUNK_SIZE;
        if (end_pixel > pixel_count) end_pixel = pixel_count;
        int chunk_size = end_pixel - start_pixel;
        
        // Convert this chunk to RGB565
        for (int i = 0; i < chunk_size; i++) {
            uint8_t color_index = DG_ScreenBuffer[start_pixel + i];
            rgb565_chunk[i] = doom_palette[color_index];
        }
        
        // Calculate row/col for this chunk
        int start_y = start_pixel / DOOMGENERIC_RESX;
        int start_x = start_pixel % DOOMGENERIC_RESX;
        
        // Push this chunk
        int rows = chunk_size / DOOMGENERIC_RESX;
        if (rows > 0) {
            M5.Lcd.pushImage(0, start_y, DOOMGENERIC_RESX, rows, rgb565_chunk);
        }
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
    // Create Doom instance with minimal arguments
    char* argv[] = {
        (char*)"doom",
        (char*)"-iwad", (char*)"doom1.wad",  // We'll need to embed this
        (char*)"-skill", (char*)"2",  // Medium difficulty
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
