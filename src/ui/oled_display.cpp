#include "oled_display.h"
#include "../../include/globals.h"
#include "../../src/utils/config.h"
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_W 128
#define SCREEN_H 64

static Adafruit_SSD1306 display(SCREEN_W, SCREEN_H, &Wire, -1);

static void draw_title(const char* t) {
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 0); display.println(t);
    display.drawLine(0, 10, 127, 10, SSD1306_WHITE);
}

static const char* trend_sym(TrendDir d) {
    switch(d) {
        case TREND_RISING_FAST: return "^^";
        case TREND_RISING:      return "^ ";
        case TREND_FALLING:     return "v ";
        default:                return "- ";
    }
}

void oled_init() {
    Wire.begin(OLED_SDA, OLED_SCL);
    if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDRESS)) {
        Serial.println("[OLED] Init FAILED!");
        return;
    }
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(10, 20); display.println("Fire & Gas System");
    display.setCursor(20, 35); display.println("Starting up...");
    display.display();
    Serial.println("[OLED] Initialized.");
}

void oled_draw_main_menu(int cursor) {
    draw_title("  FIRE MONITOR");
    display.setCursor(0, 16);
    display.print(cursor == 0 ? "> " : "  "); display.println("1. Set Threshold");
    display.setCursor(0, 28);
    display.print(cursor == 1 ? "> " : "  "); display.println("2. View Sensors");
    display.setCursor(0, 52); display.println("UP/DN=Move  SEL=Open");
    display.display();
}

void oled_draw_threshold_menu(int cursor) {
    draw_title("  SET THRESHOLD");
    display.setCursor(0, 16);
    display.print(cursor == 0 ? "> " : "  ");
    display.print("Temp: "); display.print(g_user_temp_limit); display.println(" C");
    display.setCursor(0, 28);
    display.print(cursor == 1 ? "> " : "  ");
    display.print("Gas:  "); display.print(g_user_gas_limit); display.println(" PPM");
    display.setCursor(0, 52); display.println("SEL=Edit  BACK=Back");
    display.display();
}

void oled_draw_temp_adjust(int value) {
    draw_title("  TEMP LIMIT");
    display.setTextSize(2);
    display.setCursor(25, 20); display.print(value); display.print(" C");
    display.setTextSize(1);
    display.setCursor(0, 52); display.println("UP/DN=Adj  SEL=Save");
    display.display();
}

void oled_draw_gas_adjust(int value) {
    draw_title("  GAS LIMIT");
    display.setTextSize(2);
    display.setCursor(10, 20); display.print(value);
    display.setTextSize(1); display.print(" PPM");
    display.setCursor(0, 52); display.println("UP/DN=Adj  SEL=Save");
    display.display();
}

void oled_draw_sensor_view() {
    draw_title("  LIVE SENSORS");
    display.setCursor(0, 14);
    display.print("Temp : "); display.print(g_temp_c, 1);
    display.print("C "); display.println(trend_sym(g_temp_trend));

    display.setCursor(0, 24);
    display.print("Gas  : "); display.print((int)g_gas_ppm);
    display.print(" "); display.println(trend_sym(g_gas_trend));

    display.setCursor(0, 34);
    display.print("Flame: "); display.println(g_flame ? "YES!" : "NO");

    display.setCursor(0, 44);
    display.print("Risk : ");
    if      (g_risk_level == RISK_SAFE)    display.println("SAFE");
    else if (g_risk_level == RISK_WARNING) display.println("WARNING");
    else                                   display.println("DANGER");

    if (g_early_warning && g_seconds_to_danger > 0) {
        display.setCursor(0, 54);
        display.print("~"); display.print(g_seconds_to_danger); display.print("s to DANGER!");
    }
    display.display();
}

void oled_draw_emergency(bool is_fire) {
    display.clearDisplay();
    display.setTextSize(2);
    display.setCursor(5, 5);
    display.println(is_fire ? "!! FIRE !!" : "!! SMOKE !!");
    display.setCursor(5, 30);
    display.println(is_fire ? "  ALERT!" : "DETECTED!");
    display.setTextSize(1);
    display.setCursor(5, 52); display.println("  SEL = Mute Alarm");
    display.display();
}

void oled_draw_alarm_muted() {
    draw_title("  ALARM MUTED");
    display.setCursor(0, 14);
    display.print("Temp : "); display.print(g_temp_c, 1); display.println(" C");
    display.setCursor(0, 24);
    display.print("Gas  : "); display.print((int)g_gas_ppm); display.println(" PPM");
    display.setCursor(0, 34);
    display.print("Flame: "); display.println(g_flame ? "YES!" : "NO");
    display.setCursor(0, 48);
    display.println("Monitoring active...");
    display.display();
}
