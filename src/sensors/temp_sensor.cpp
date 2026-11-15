#include "temp_sensor.h"
#include "../utils/config.h"
#include "DHT.h"

// Create DHT object
static DHT dht(DHT_PIN, DHT_TYPE);

// Store last valid readings (in case sensor fails)
static float last_temp = 0.0;
static float last_humidity = 0.0;

void temp_init() {
    dht.begin();
    Serial.println("[TEMP] DHT Sensor initialized.");
}

float temp_read_celsius() {
    float t = dht.readTemperature();  // Celsius
    if (isnan(t)) {
        Serial.println("[TEMP] Read failed! Using last value.");
        return last_temp;             // Return last valid reading
    }
    last_temp = t;
    return t;
}

float temp_read_humidity() {
    float h = dht.readHumidity();
    if (isnan(h)) {
        return last_humidity;
    }
    last_humidity = h;
    return h;
}
