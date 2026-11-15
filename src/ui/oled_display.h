#ifndef OLED_DISPLAY_H
#define OLED_DISPLAY_H
#include "../../include/globals.h"

void oled_init();
void oled_draw_main_menu(int cursor);
void oled_draw_threshold_menu(int cursor);
void oled_draw_temp_adjust(int value);
void oled_draw_gas_adjust(int value);
void oled_draw_sensor_view();
void oled_draw_emergency(bool is_fire);
void oled_draw_alarm_muted();

#endif
