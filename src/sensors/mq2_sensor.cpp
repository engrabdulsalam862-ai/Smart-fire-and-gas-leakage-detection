#include "mq2_sensor.h"
#include "../utils/config.h"
#include "Arduino.h"

// Ro will be calculated during calibration in clean air
static float Ro = 9.83;   // Default, will be updated after calibration

// ===== INIT + CALIBRATE =====
void mq2_init() {
    pinMode(MQ2_PIN, INPUT);

    Serial.println("[MQ2] Warming up... (30 seconds)");
    for (int i = 30; i > 0; i--) {
        Serial.print("[MQ2] Warm-up: ");
        Serial.print(i);
        Serial.println(" sec remaining...");
        delay(1000);
    }

    // Auto-calibrate Ro in clean air (average of 50 readings)
    Serial.println("[MQ2] Calibrating Ro in clean air...");
    float rs_sum = 0;
    for (int i = 0; i < 50; i++) {
        int raw = analogRead(MQ2_PIN);
        float voltage = raw * (3.3f / 4095.0f);
        if (voltage < 0.01f) voltage = 0.01f;   // prevent divide by zero
        float rs = ((3.3f - voltage) / voltage) * 10.0f;
        rs_sum += rs;
        delay(100);
    }
    float rs_clean = rs_sum / 50.0f;

    // In clean air, Rs/Ro = 9.8 for LPG/Smoke curve
    Ro = rs_clean / 9.8f;

    Serial.print("[MQ2] Calibration done. Ro = ");
    Serial.println(Ro);
    Serial.println("[MQ2] Ready.");
}

// ===== READ PPM =====
float mq2_read_ppm() {
    int raw = analogRead(MQ2_PIN);

    // Print raw ADC for debugging
    Serial.print("[MQ2] Raw ADC: ");
    Serial.print(raw);

    float voltage = raw * (3.3f / 4095.0f);
    if (voltage < 0.01f) voltage = 0.01f;

    float Rs = ((3.3f - voltage) / voltage) * 10.0f;
    float ratio = Rs / Ro;

    // PPM from datasheet curve: PPM = 574.25 * (Rs/Ro)^-2.222
    float ppm = 574.25f * pow(ratio, -2.222f);

    // Clamp to realistic range
    if (ppm < 0)     ppm = 0;
    if (ppm > 9999)  ppm = 9999;

    Serial.print("  |  Ratio: ");
    Serial.print(ratio);
    Serial.print("  |  PPM: ");
    Serial.println(ppm);

    return ppm;
}
