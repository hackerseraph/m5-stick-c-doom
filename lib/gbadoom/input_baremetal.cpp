// Bare-metal button and IMU input for M5StickC Plus
#include <Arduino.h>
#include <Wire.h>
#include "input_baremetal.h"

// M5StickC Plus button pins
#define BTN_A_PIN   37
#define BTN_B_PIN   39

// MPU6886 I2C
#define MPU6886_ADDR    0x68
#define MPU6886_WHO_AM_I    0x75
#define MPU6886_ACCEL_XOUT_H 0x3B
#define MPU6886_PWR_MGMT_1   0x6B

static uint32_t button_state = 0;
static bool imu_initialized = false;

// Button masks
#define BTN_A_MASK  (1 << 0)
#define BTN_B_MASK  (1 << 1)

// Key definitions
#define KEY_FIRE   'z'
#define KEY_USE    'x'
#define KEY_ESCAPE 27

static void mpu6886_write_byte(uint8_t reg, uint8_t data) {
    Wire.beginTransmission(MPU6886_ADDR);
    Wire.write(reg);
    Wire.write(data);
    Wire.endTransmission();
}

static uint8_t mpu6886_read_byte(uint8_t reg) {
    Wire.beginTransmission(MPU6886_ADDR);
    Wire.write(reg);
    Wire.endTransmission(false);
    Wire.requestFrom(MPU6886_ADDR, 1);
    return Wire.read();
}

static void mpu6886_read_bytes(uint8_t reg, uint8_t* buf, uint8_t len) {
    Wire.beginTransmission(MPU6886_ADDR);
    Wire.write(reg);
    Wire.endTransmission(false);
    Wire.requestFrom(MPU6886_ADDR, len);
    for (int i = 0; i < len; i++) {
        buf[i] = Wire.read();
    }
}

extern "C" {

void input_init(void) {
    // Initialize buttons
    pinMode(BTN_A_PIN, INPUT);
    pinMode(BTN_B_PIN, INPUT);
    
    // Initialize I2C for IMU
    Wire.begin(21, 22);  // SDA=21, SCL=22
    Wire.setClock(400000);
    
    // Initialize MPU6886
    uint8_t whoami = mpu6886_read_byte(MPU6886_WHO_AM_I);
    if (whoami == 0x19) {
        // Wake up MPU6886
        mpu6886_write_byte(MPU6886_PWR_MGMT_1, 0x00);
        delay(10);
        
        // Configure accelerometer (±8g)
        mpu6886_write_byte(0x1C, 0x10);
        
        // Configure gyroscope (±2000dps)
        mpu6886_write_byte(0x1B, 0x18);
        
        imu_initialized = true;
        Serial.println("IMU initialized: MPU6886");
    } else {
        Serial.printf("IMU not found (WHO_AM_I=0x%02X)\n", whoami);
    }
    
    Serial.println("Input initialized");
}

uint32_t input_get_buttons(void) {
    uint32_t state = 0;
    
    // Read buttons (active LOW)
    if (digitalRead(BTN_A_PIN) == LOW) state |= BTN_A_MASK;
    if (digitalRead(BTN_B_PIN) == LOW) state |= BTN_B_MASK;
    
    button_state = state;
    return state;
}

int input_get_key(void) {
    uint32_t buttons = input_get_buttons();
    
    if (buttons & BTN_A_MASK) return KEY_FIRE;
    if (buttons & BTN_B_MASK) return KEY_USE;
    
    return 0;
}

void input_get_accel(float* ax, float* ay, float* az) {
    if (!imu_initialized) {
        *ax = *ay = *az = 0.0f;
        return;
    }
    
    uint8_t buf[6];
    mpu6886_read_bytes(MPU6886_ACCEL_XOUT_H, buf, 6);
    
    int16_t raw_x = (buf[0] << 8) | buf[1];
    int16_t raw_y = (buf[2] << 8) | buf[3];
    int16_t raw_z = (buf[4] << 8) | buf[5];
    
    // Convert to g (±8g range)
    *ax = raw_x / 4096.0f;
    *ay = raw_y / 4096.0f;
    *az = raw_z / 4096.0f;
}

} // extern "C"
