#ifndef IOT_MANAGER_H
#define IOT_MANAGER_H
#include "../../include/globals.h"

void iot_init();
void iot_update();     // Call every loop — uploads on 15s interval
bool iot_connected();

#endif
