#ifndef GLOBALS_H
#define GLOBALS_H

// ===== Shared Enums =====
enum RiskLevel  { RISK_SAFE = 0, RISK_WARNING = 1, RISK_DANGER = 2 };
enum TrendDir   { TREND_STABLE = 0, TREND_RISING = 1, TREND_RISING_FAST = 2, TREND_FALLING = 3 };

// ===== Shared Global Variables =====
extern float     g_gas_ppm;
extern float     g_temp_c;
extern float     g_humidity;
extern bool      g_flame;

extern RiskLevel g_risk_level;
extern TrendDir  g_gas_trend;
extern TrendDir  g_temp_trend;
extern bool      g_early_warning;
extern int       g_seconds_to_danger;
extern bool      g_alarm_muted;
extern float     g_fuzzy_score;    // 0-100 weighted risk score

// User-adjustable thresholds (changeable via OLED menu)
extern int       g_user_gas_limit;    // PPM  (default 400)
extern int       g_user_temp_limit;   // C    (default 55)

#endif
