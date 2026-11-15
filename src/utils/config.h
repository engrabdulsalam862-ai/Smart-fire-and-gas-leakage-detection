#ifndef CONFIG_H
#define CONFIG_H

#include "DHT.h"

// =========================================================
//   PIN DEFINITIONS
// =========================================================

// ----- Sensors -----
#define MQ2_PIN             34      // MQ2 Gas Sensor      (Analog)
#define FLAME_PIN           26      // Flame Sensor        (Digital only)
#define DHT_PIN             27      // DHT11/DHT22         (Data pin)
#define DHT_TYPE            DHT11   // Change to DHT22 if needed

// ----- Buzzer -----
#define BUZZER_PIN          25

// Individual LEDs NOT used — RGB LED handles all colors

// ----- RGB LED Pins (Common Cathode) -----
#define RGB_R_PIN           5       // RGB Red   channel
#define RGB_G_PIN           4       // RGB Green channel
#define RGB_B_PIN           18      // RGB Blue  channel

// ----- OLED Display (I2C) -----
#define OLED_SDA            21      // I2C Data
#define OLED_SCL            22      // I2C Clock
#define OLED_ADDRESS        0x3C    // Default I2C address for 0.96" OLED

// ----- Push Buttons -----
#define BTN_UP              32
#define BTN_DOWN            33
#define BTN_SELECT          14
#define BTN_BACK            13      // GPIO12 OK after boot (INPUT_PULLUP = HIGH = safe)

// =========================================================
//   AI THRESHOLDS — Risk Level Classification
// =========================================================
#define DANGER_SCORE        70      // Score >= 70 → DANGER
#define WARNING_SCORE       40      // Score >= 40 → WARNING

// =========================================================
//   FUZZY WEIGHTS — Must add up to 1.0
// =========================================================
#define GAS_WEIGHT          0.45f   // Gas is primary indicator
#define FLAME_WEIGHT        0.35f   // Flame directly confirms fire
#define TEMP_WEIGHT         0.20f   // Temperature is supporting

// =========================================================
//   USER ADJUSTABLE THRESHOLDS (changeable via menu)
// =========================================================
#define DEFAULT_TEMP_LIMIT  30      // °C  — user can change via OLED menu
#define DEFAULT_GAS_LIMIT   400     // PPM — user can change via OLED menu

// Raw sensor danger thresholds (for basic testing)
#define GAS_WARNING_PPM     300
#define GAS_DANGER_PPM      500
#define TEMP_WARNING_C      45.0f
#define TEMP_DANGER_C       60.0f

// =========================================================
//   RGB LED COLOR CODES (R, G, B values 0–255)
// =========================================================
#define RGB_SAFE_R          0
#define RGB_SAFE_G          255
#define RGB_SAFE_B          0       // Green

#define RGB_WARNING_R       255
#define RGB_WARNING_G       165
#define RGB_WARNING_B       0       // Orange

#define RGB_DANGER_R        255
#define RGB_DANGER_G        0
#define RGB_DANGER_B        0       // Red

// =========================================================
//   IoT SETTINGS
// =========================================================
#define WIFI_SSID           "YOUR_WIFI_SSID"
#define WIFI_PASS           "YOUR_WIFI_PASSWORD"
#define THINGSPEAK_API      "YOUR_THINGSPEAK_API_KEY"
#define THINGSPEAK_CH_ID    3387227           // Your ThingSpeak Channel ID
#define IOT_UPLOAD_INTERVAL 15000       // Upload every 15 seconds (ms)

#endif
