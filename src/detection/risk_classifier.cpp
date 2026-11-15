#include "risk_classifier.h"
#include "../../include/globals.h"

// Override rules (applied AFTER fuzzy score):
// 1. Flame detected alone → always at least WARNING
// 2. Flame + any gas reading > 50 PPM → force DANGER
// 3. Fuzzy score 35-40 with flame → promote to WARNING
RiskLevel risk_apply_override(RiskLevel fuzzy_level, bool flame, float gas_ppm) {

    if (flame) {
        // Flame + gas → confirmed fire = DANGER
        if (gas_ppm > 50.0f) return RISK_DANGER;

        // Flame alone → at minimum WARNING
        if (fuzzy_level == RISK_SAFE) return RISK_WARNING;
    }

    return fuzzy_level;
}
