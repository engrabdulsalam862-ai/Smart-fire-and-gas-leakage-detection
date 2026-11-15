#include "menu_manager.h"
#include "oled_display.h"
#include "../../include/globals.h"
#include "../../src/alerts/alert_manager.h"
#include "Arduino.h"

static MenuScreen current_screen  = SCREEN_MAIN_MENU;
static int  main_cursor    = 0;
static int  thresh_cursor  = 0;
static int  temp_edit      = 0;
static int  gas_edit       = 0;

// Periodic redraw timer (for live sensor view)
static unsigned long last_oled_ms  = 0;
#define OLED_REFRESH_MS  500

// ── helpers ────────────────────────────────────────────
static void go_to(MenuScreen s) {
    current_screen = s;
    last_oled_ms   = 0;   // force immediate redraw on next tick
}

static bool oled_due() {
    if (millis() - last_oled_ms >= OLED_REFRESH_MS) {
        last_oled_ms = millis();
        return true;
    }
    return false;
}

// ── Emergency override ────────────────────────────────────
static void check_emergency() {
    // Only auto-jump to emergency if DANGER and not already there
    if (g_risk_level == RISK_DANGER &&
        current_screen != SCREEN_EMERGENCY &&
        current_screen != SCREEN_ALARM_MUTED) {
        go_to(SCREEN_EMERGENCY);
    }
    // Auto-clear muted screen when safe again
    if (current_screen == SCREEN_ALARM_MUTED && g_risk_level == RISK_SAFE) {
        go_to(SCREEN_MAIN_MENU);
    }
}

// ── INIT ─────────────────────────────────────────────────
void menu_init() {
    current_screen = SCREEN_MAIN_MENU;
    main_cursor    = 0;
    thresh_cursor  = 0;
    last_oled_ms   = 0;
    Serial.println("[MENU] Initialized.");
}

MenuScreen menu_get_screen() { return current_screen; }

// ── UPDATE ────────────────────────────────────────────────
void menu_update(ButtonPress btn) {

    // Emergency check — may change current_screen
    check_emergency();

    bool btn_pressed = (btn != BTN_NONE);

    // Debug: log button + screen
    if (btn_pressed) {
        Serial.print("[MENU] Screen="); Serial.print((int)current_screen);
        Serial.print(" Btn="); Serial.println((int)btn);
    }

    switch (current_screen) {

        // ── MAIN MENU ─────────────────────────────────────
        case SCREEN_MAIN_MENU:
            if (btn == BTN_UP_P || btn == BTN_DOWN_P) {
                main_cursor = 1 - main_cursor;
            }
            if (btn == BTN_SEL_P) {
                go_to(main_cursor == 0 ? SCREEN_THRESHOLD_MENU : SCREEN_VIEW_SENSORS);
            }
            // Draw on button press or periodic refresh
            if (btn_pressed || oled_due()) {
                oled_draw_main_menu(main_cursor);
            }
            break;

        // ── THRESHOLD MENU ────────────────────────────────
        case SCREEN_THRESHOLD_MENU:
            if (btn == BTN_UP_P || btn == BTN_DOWN_P) thresh_cursor = 1 - thresh_cursor;
            if (btn == BTN_SEL_P) {
                if (thresh_cursor == 0) {
                    temp_edit = g_user_temp_limit;
                    go_to(SCREEN_THRESHOLD_TEMP);
                } else {
                    gas_edit = g_user_gas_limit;
                    go_to(SCREEN_THRESHOLD_GAS);
                }
            }
            if (btn == BTN_BACK_P) go_to(SCREEN_MAIN_MENU);
            if (btn_pressed || oled_due()) oled_draw_threshold_menu(thresh_cursor);
            break;

        // ── TEMP ADJUST ───────────────────────────────────
        case SCREEN_THRESHOLD_TEMP:
            if (btn == BTN_UP_P)   temp_edit = min(temp_edit + 1, 100);
            if (btn == BTN_DOWN_P) temp_edit = max(temp_edit - 1, 35);
            if (btn == BTN_SEL_P)  { g_user_temp_limit = temp_edit; go_to(SCREEN_THRESHOLD_MENU); }
            if (btn == BTN_BACK_P) go_to(SCREEN_THRESHOLD_MENU);
            if (btn_pressed || oled_due()) oled_draw_temp_adjust(temp_edit);
            break;

        // ── GAS ADJUST ────────────────────────────────────
        case SCREEN_THRESHOLD_GAS:
            if (btn == BTN_UP_P)   gas_edit = min(gas_edit + 10, 2000);
            if (btn == BTN_DOWN_P) gas_edit = max(gas_edit - 10, 100);
            if (btn == BTN_SEL_P)  { g_user_gas_limit = gas_edit; go_to(SCREEN_THRESHOLD_MENU); }
            if (btn == BTN_BACK_P) go_to(SCREEN_THRESHOLD_MENU);
            if (btn_pressed || oled_due()) oled_draw_gas_adjust(gas_edit);
            break;

        // ── SENSOR VIEW ───────────────────────────────────
        case SCREEN_VIEW_SENSORS:
            if (btn == BTN_BACK_P) {
                go_to(SCREEN_MAIN_MENU);
                break;          // Exit immediately — don't draw sensor view
            }
            if (oled_due()) oled_draw_sensor_view();  // Refresh every 500ms
            break;

        // ── EMERGENCY ─────────────────────────────────────
        case SCREEN_EMERGENCY:
            if (oled_due()) oled_draw_emergency(g_flame);
            if (btn == BTN_SEL_P) {
                alert_mute();
                go_to(SCREEN_ALARM_MUTED);
            }
            break;

        // ── ALARM MUTED ───────────────────────────────────
        case SCREEN_ALARM_MUTED:
            if (oled_due()) oled_draw_alarm_muted();
            break;
    }
}
