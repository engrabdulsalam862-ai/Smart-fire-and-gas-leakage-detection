#include "flame_sensor.h"
#include "../utils/config.h"
#include "Arduino.h"

void flame_init() {
    pinMode(FLAME_PIN, INPUT);
    Serial.println("[FLAME] Sensor initialized. (Digital only)");
}

// LOW = Flame detected, HIGH = No flame
bool flame_detected() {
    return (digitalRead(FLAME_PIN) == LOW);
}
