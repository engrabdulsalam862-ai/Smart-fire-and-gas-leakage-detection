#ifndef FUZZY_CLASSIFIER_H
#define FUZZY_CLASSIFIER_H
#include "../../include/globals.h"

struct FuzzyResult {
    float gas_score;    // 0-100
    float flame_score;  // 0-100
    float temp_score;   // 0-100
    float total_score;  // 0-100 (weighted)
    RiskLevel level;    // SAFE / WARNING / DANGER
};

FuzzyResult fuzzy_classify(float gas_ppm, bool flame, float temp_c);
void        fuzzy_print(const FuzzyResult& r);

#endif
