#include "Arduino.h"
#include "sensors/mq2_sensor.h"
#include "sensors/flame_sensor.h"
#include "sensors/temp_sensor.h"
#include "detection/fuzzy_classifier.h"
#include "detection/risk_classifier.h"
#include "trend_analysis/trend_engine.h"
#include "alerts/alert_manager.h"
#include "ui/button_handler.h"
#include "ui/oled_display.h"
#include "ui/menu_manager.h"
#include "iot/iot_manager.h"
#include "utils/config.h"
#include "../include/globals.h"

// ===== Global variable definitions =====
float     g_gas_ppm           = 0;
float     g_temp_c            = 25.0;
float     g_humidity          = 50.0;
bool      g_flame             = false;
RiskLevel g_risk_level        = RISK_SAFE;
TrendDir  g_gas_trend         = TREND_STABLE;
TrendDir  g_temp_trend        = TREND_STABLE;
bool      g_early_warning     = false;
int       g_seconds_to_danger = -1;
bool      g_alarm_muted       = false;
float     g_fuzzy_score       = 0.0f;
int       g_user_gas_limit    = 400;
int       g_user_temp_limit   = 55;

static int  flame_count  = 0;
static bool system_ready = false;

static unsigned long last_sensor_ms = 0;
#define SENSOR_INTERVAL  1000

// ===== SETUP =====
void setup() {
    Serial.begin(115200);

    // Force all outputs OFF immediately
    pinMode(BUZZER_PIN, OUTPUT); digitalWrite(BUZZER_PIN, LOW);
    pinMode(RGB_R_PIN,  OUTPUT); analogWrite(RGB_R_PIN, 0);
    pinMode(RGB_G_PIN,  OUTPUT); analogWrite(RGB_G_PIN, 0);
    pinMode(RGB_B_PIN,  OUTPUT); analogWrite(RGB_B_PIN, 0);

    Serial.println("=================================");
    Serial.println("  Fire & Gas Detection System");
    Serial.println("=================================");

    mq2_init();          // 30s warmup + Ro calibration
    flame_init();
    temp_init();
    trend_init();
    alert_init();
    button_init();
    oled_init();
    menu_init();
    iot_init();          // WiFi + ThingSpeak connect

    Serial.print("[CONFIG] Gas Limit : "); Serial.print(g_user_gas_limit);  Serial.println(" PPM");
    Serial.print("[CONFIG] Temp Limit: "); Serial.print(g_user_temp_limit); Serial.println(" C");
    Serial.print("[IoT]    Status    : "); Serial.println(iot_connected() ? "Connected" : "Offline");

    // Ready beep + green LED
    digitalWrite(BUZZER_PIN, HIGH); delay(150);
    digitalWrite(BUZZER_PIN, LOW);
    analogWrite(RGB_G_PIN, 255);

    system_ready   = true;
    last_sensor_ms = millis();
    Serial.println("[SYSTEM] Ready — Full pipeline active.\n");
}

// ===== LOOP — Fast (no delay) =====
void loop() {
    unsigned long now = millis();

    // ── 1. BUTTONS — instant response ──
    ButtonPress btn = button_read();
    if (btn == BTN_SEL_P && g_risk_level != RISK_SAFE) {
        alert_mute();
    }

    // ── 2. OLED MENU ──
    menu_update(btn);

    // ── 3. FULL PIPELINE — every 1 second ──
    if (now - last_sensor_ms >= SENSOR_INTERVAL) {
        last_sensor_ms = now;

        // Step 1: Read sensors
        g_gas_ppm  = mq2_read_ppm();
        g_temp_c   = temp_read_celsius();
        g_humidity = temp_read_humidity();

        // Flame: 3 consecutive = confirmed
        if (flame_detected()) { if (flame_count < 10) flame_count++; }
        else                  { flame_count = 0; }
        g_flame = (flame_count >= 3);

        // Step 2: Trend analysis
        trend_update(g_gas_ppm, g_temp_c);

        if (system_ready) {
            // Step 3: Fuzzy weighted classifier (AI core)
            FuzzyResult fr  = fuzzy_classify(g_gas_ppm, g_flame, g_temp_c);
            g_fuzzy_score   = fr.total_score;
            g_risk_level    = fr.level;

            // Step 4: Override rule (flame + gas > 300 → force DANGER)
            g_risk_level = risk_apply_override(g_risk_level, g_flame, g_gas_ppm);

            // Step 5: Trend early warning promotion
            if (g_risk_level == RISK_SAFE && g_early_warning) {
                g_risk_level = RISK_WARNING;
            }

            // Step 6: Alert outputs (RGB + Buzzer)
            alert_update(g_risk_level, g_early_warning);

            // Step 7: Serial log
            fuzzy_print(fr);
            Serial.print("[GAS] ");     Serial.print(g_gas_ppm, 0);       Serial.print(" PPM");
            Serial.print(" | [TEMP] "); Serial.print(g_temp_c, 1);        Serial.print(" C");
            Serial.print(" | [FLAME] ");Serial.print(g_flame ? "YES" : "NO");
            Serial.print(" | Score=");  Serial.print(g_fuzzy_score, 1);
            Serial.print(" | [RISK] ");
            if      (g_risk_level == RISK_SAFE)    Serial.print("SAFE");
            else if (g_risk_level == RISK_WARNING) Serial.print("WARNING");
            else                                   Serial.print("DANGER");
            if (g_alarm_muted)   Serial.print(" [MUTED]");
            if (g_early_warning) { Serial.print(" | ~"); Serial.print(g_seconds_to_danger); Serial.print("s"); }
            Serial.println();
        }
    }

    // ── 4. IoT UPLOAD — every 15 seconds ──
    iot_update();
}
