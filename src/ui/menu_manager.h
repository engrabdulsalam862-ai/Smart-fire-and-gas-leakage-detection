#ifndef MENU_MANAGER_H
#define MENU_MANAGER_H
#include "../../include/globals.h"
#include "button_handler.h"

enum MenuScreen {
    SCREEN_MAIN_MENU,
    SCREEN_THRESHOLD_MENU,
    SCREEN_THRESHOLD_TEMP,
    SCREEN_THRESHOLD_GAS,
    SCREEN_VIEW_SENSORS,
    SCREEN_EMERGENCY,
    SCREEN_ALARM_MUTED
};

void       menu_init();
void       menu_update(ButtonPress btn);
MenuScreen menu_get_screen();

#endif
