#include "trend_engine.h"
#include "../../include/globals.h"
#include "../../src/utils/config.h"
#include "Arduino.h"

#define BUFFER_SIZE 5

static float gas_buf[BUFFER_SIZE]  = {0};
static float temp_buf[BUFFER_SIZE] = {0};
static int   buf_index = 0;
static int   buf_count = 0;

void trend_init() {
    buf_index = 0;
    buf_count = 0;
    Serial.println("[TREND] Initialized.");
}

static TrendDir classify(float rate) {
    if      (rate >  15.0f) return TREND_RISING_FAST;
    else if (rate >   3.0f) return TREND_RISING;
    else if (rate <  -3.0f) return TREND_FALLING;
    else                    return TREND_STABLE;
}

void trend_update(float gas_ppm, float temp_c) {
    gas_buf[buf_index]  = gas_ppm;
    temp_buf[buf_index] = temp_c;
    buf_index = (buf_index + 1) % BUFFER_SIZE;
    if (buf_count < BUFFER_SIZE) buf_count++;

    if (buf_count < 2) {
        g_gas_trend = g_temp_trend = TREND_STABLE;
        g_early_warning     = false;
        g_seconds_to_danger = -1;
        return;
    }

    int oldest = buf_index % BUFFER_SIZE;
    int latest = (buf_index - 1 + BUFFER_SIZE) % BUFFER_SIZE;
    int span   = buf_count - 1;

    float gas_rate  = (gas_buf[latest]  - gas_buf[oldest])  / (float)span;
    float temp_rate = (temp_buf[latest] - temp_buf[oldest]) / (float)span;

    g_gas_trend  = classify(gas_rate);
    g_temp_trend = classify(temp_rate);

    // Default state
    g_early_warning     = false;
    g_seconds_to_danger = -1;

    // Corrected condition: Trigger on RISING or RISING_FAST if slope is positive
    if (gas_ppm < g_user_gas_limit && (g_gas_trend == TREND_RISING || g_gas_trend == TREND_RISING_FAST) && gas_rate > 0.5f) {
        g_seconds_to_danger = (int)((g_user_gas_limit - gas_ppm) / gas_rate);
        g_early_warning = true;
    }
}