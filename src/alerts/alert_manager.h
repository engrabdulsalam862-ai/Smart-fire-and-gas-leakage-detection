#ifndef ALERT_MANAGER_H
#define ALERT_MANAGER_H
#include "../../include/globals.h"

void alert_init();
void alert_update(RiskLevel level, bool early_warning);
void alert_mute();

#endif
