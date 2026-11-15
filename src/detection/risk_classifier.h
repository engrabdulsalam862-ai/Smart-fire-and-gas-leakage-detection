#ifndef RISK_CLASSIFIER_H
#define RISK_CLASSIFIER_H
#include "../../include/globals.h"

// Override rule: flame + gas > 300 PPM → force DANGER
RiskLevel risk_apply_override(RiskLevel fuzzy_level, bool flame, float gas_ppm);

#endif
