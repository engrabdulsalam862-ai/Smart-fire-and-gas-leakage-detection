#include "fuzzy_classifier.h"
#include "../../src/utils/config.h"
#include "../../include/globals.h"
#include "Arduino.h"
// =========================================================
//   NORMALIZATION RANGES
// =========================================================
// Gas PPM:  0 = safe, 1000 = fully dangerous
#define GAS_SAFE_PPM     200.0f
#define GAS_WARN_PPM     400.0f
#define GAS_DANGER_PPM   600.0f
#define GAS_MAX_PPM     1000.0f
// Temperature: 25°C = safe, 70°C+ = fully dangerous
#define TEMP_SAFE_C      30.0f
#define TEMP_WARN_C      44.0f
#define TEMP_DANGER_C    55.0f
#define TEMP_MAX_C       80.0f
// Fuzzy weights (must sum to 1.0)
#define W_GAS    0.45f
#define W_FLAME  0.35f
#define W_TEMP   0.20f
// Score thresholds
#define SCORE_DANGER   70.0f
#define SCORE_WARNING  40.0f
// =========================================================
//   HELPER — Linear interpolation clamp to 0-100
// =========================================================
static float normalize(float value, float low, float high) {
    if (value <= low)  return 0.0f;
    if (value >= high) return 100.0f;
    return ((value - low) / (high - low)) * 100.0f;
}
// =========================================================
//   GAS SCORE — Piecewise fuzzy membership
//   0–200 PPM    → low score  (0–15)
//   200–400 PPM  → medium     (15–50)
//   400–600 PPM  → high       (50–85)
//   600+ PPM     → very high  (85–100)
// =========================================================
static float gas_fuzzy_score(float ppm) {
    if (ppm < GAS_SAFE_PPM)   return normalize(ppm, 0,            GAS_SAFE_PPM)   * 0.15f;
    if (ppm < GAS_WARN_PPM)   return 15.0f + normalize(ppm, GAS_SAFE_PPM, GAS_WARN_PPM)   * 0.35f;
    if (ppm < GAS_DANGER_PPM) return 50.0f + normalize(ppm, GAS_WARN_PPM, GAS_DANGER_PPM) * 0.35f;
    return                           85.0f + normalize(ppm, GAS_DANGER_PPM, GAS_MAX_PPM)   * 0.15f;
}
// =========================================================
//   FLAME SCORE — Binary + boost
//   NO flame  → 0
//   YES flame → 100 (direct danger confirmation)
// =========================================================
static float flame_fuzzy_score(bool flame) {
    return flame ? 100.0f : 0.0f;
}
// =========================================================
//   TEMP SCORE — Linear fuzzy membership
//   <30°C  → 0
//   30–44  → low  (0–30)
//   44–55  → mid  (30–75)
//   55–80  → high (75–100)
// =========================================================
static float temp_fuzzy_score(float temp) {
    if (temp < TEMP_SAFE_C)   return 0.0f;
    if (temp < TEMP_WARN_C)   return normalize(temp, TEMP_SAFE_C,  TEMP_WARN_C)   * 0.30f;
    if (temp < TEMP_DANGER_C) return 30.0f + normalize(temp, TEMP_WARN_C, TEMP_DANGER_C) * 0.45f;
    return                           75.0f + normalize(temp, TEMP_DANGER_C, TEMP_MAX_C)  * 0.25f;
}
// =========================================================
//   MAIN CLASSIFIER
// =========================================================
FuzzyResult fuzzy_classify(float gas_ppm, bool flame, float temp_c) {
    FuzzyResult r;

    // Step 1: Normalize each sensor to 0-100
    r.gas_score   = gas_fuzzy_score(gas_ppm);
    r.flame_score = flame_fuzzy_score(flame);
    r.temp_score  = temp_fuzzy_score(temp_c);

    // Step 2: Weighted sum
    //   Gas=45%, Flame=35%, Temp=20%
    r.total_score = (r.gas_score * W_GAS)
                  + (r.flame_score * W_FLAME)
                  + (r.temp_score * W_TEMP);

    // Step 3: Classify
    if      (r.total_score >= SCORE_DANGER)  r.level = RISK_DANGER;
    else if (r.total_score >= SCORE_WARNING) r.level = RISK_WARNING;
    else                                     r.level = RISK_SAFE;

    return r;
}

// =========================================================
//   DEBUG PRINT
// =========================================================
void fuzzy_print(const FuzzyResult& r) {
    Serial.print("[FUZZY] Gas=");   Serial.print(r.gas_score,   1);
    Serial.print(" Flame=");        Serial.print(r.flame_score, 1);
    Serial.print(" Temp=");         Serial.print(r.temp_score,  1);
    Serial.print(" | Score=");      Serial.print(r.total_score, 1);
    Serial.print(" | Level=");
    if      (r.level == RISK_SAFE)    Serial.println("SAFE");
    else if (r.level == RISK_WARNING) Serial.println("WARNING");
    else                              Serial.println("DANGER");
}
