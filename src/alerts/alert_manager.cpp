#include "alert_manager.h"
#include "../../include/globals.h"
#include "../../src/utils/config.h"
#include "Arduino.h"

static unsigned long last_beep_ms  = 0;
static RiskLevel     last_danger   = RISK_SAFE;  // track when NEW danger starts

static void rgb_set(int r, int g, int b) {
    analogWrite(RGB_R_PIN, r);
    analogWrite(RGB_G_PIN, g);
    analogWrite(RGB_B_PIN, b);
}

void alert_init() {
    pinMode(BUZZER_PIN, OUTPUT);
    pinMode(RGB_R_PIN,  OUTPUT);
    pinMode(RGB_G_PIN,  OUTPUT);
    pinMode(RGB_B_PIN,  OUTPUT);
    digitalWrite(BUZZER_PIN, LOW);
    rgb_set(0, 255, 0);
    g_alarm_muted = false;
    Serial.println("[ALERT] Initialized.");
}

void alert_mute() {
    g_alarm_muted = true;
    digitalWrite(BUZZER_PIN, LOW);
    Serial.println("[ALERT] Alarm MUTED by user.");
}

void alert_update(RiskLevel level, bool early_warning) {
    // Reset mute ONLY when danger fully clears (not on momentary safe)
    if (level == RISK_SAFE && last_danger == RISK_SAFE) {
        g_alarm_muted = false;
    }
    last_danger = level;

    if (level == RISK_DANGER) {
        rgb_set(RGB_DANGER_R, RGB_DANGER_G, RGB_DANGER_B);  // Red
        if (!g_alarm_muted) {
            // Pure Non-Blocking Fast Beep (Cycle of 300ms: 150ms ON, 150ms OFF)
            if (millis() - last_beep_ms > 300) {
                last_beep_ms = millis();
            }
            if (millis() - last_beep_ms < 150) {
                digitalWrite(BUZZER_PIN, HIGH);
            } else {
                digitalWrite(BUZZER_PIN, LOW);
            }
        } else {
            digitalWrite(BUZZER_PIN, LOW);
        }
    }
    else if (level == RISK_WARNING || early_warning) {
        rgb_set(RGB_WARNING_R, RGB_WARNING_G, RGB_WARNING_B);  // Orange
        if (!g_alarm_muted) {
            // Pure Non-Blocking Slow Beep (Cycle of 2000ms: 200ms ON, 1800ms OFF)
            if (millis() - last_beep_ms > 2000) {
                last_beep_ms = millis();
            }
            if (millis() - last_beep_ms < 200) {
                digitalWrite(BUZZER_PIN, HIGH);
            } else {
                digitalWrite(BUZZER_PIN, LOW);
            }
        } else {
            digitalWrite(BUZZER_PIN, LOW);
        }
    }
    else {
        rgb_set(RGB_SAFE_R, RGB_SAFE_G, RGB_SAFE_B);  // Green
        digitalWrite(BUZZER_PIN, LOW);
    }
}