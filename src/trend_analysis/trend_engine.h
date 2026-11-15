#ifndef TREND_ENGINE_H
#define TREND_ENGINE_H
#include "../../include/globals.h"

void trend_init();
void trend_update(float gas_ppm, float temp_c);

#endif
