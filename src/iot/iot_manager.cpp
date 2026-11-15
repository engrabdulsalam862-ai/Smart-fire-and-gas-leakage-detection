#include "iot_manager.h"
#include "../../include/globals.h"
#include "../../src/utils/config.h"
#include "Arduino.h"
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>

static unsigned long last_upload_ms   = 0;
static unsigned long last_reconnect_ms = 0;

// ===== WiFi Connect =====
static void wifi_connect() {
    Serial.print("[IoT] Connecting to: "); Serial.println(WIFI_SSID);
    WiFi.disconnect(true);
    delay(500);
    WiFi.begin(WIFI_SSID, WIFI_PASS);
    int attempts = 0;
    while (WiFi.status() != WL_CONNECTED && attempts < 30) {
        delay(500); Serial.print("."); attempts++;
    }
    if (WiFi.status() == WL_CONNECTED) {
        Serial.println("\n[IoT] Connected! IP: " + WiFi.localIP().toString());
    } else {
        Serial.println("\n[IoT] WiFi FAILED — offline mode.");
    }
}

void iot_init() {
    WiFi.mode(WIFI_STA);
    wifi_connect();
}

bool iot_connected() {
    return (WiFi.status() == WL_CONNECTED);
}

// ===== THINGSPEAK UPLOAD =====
// Fields:
//   1=Temp  2=Gas_PPM  3=Flame  4=FuzzyScore  5=RiskLevel  6=GasTrend
static void thingspeak_upload() {
    if (!iot_connected()) return;

    // Build URL
    String url  = "https://api.thingspeak.com/update";
    url += "?api_key="; url += THINGSPEAK_API;
    url += "&field1=";  url += String(g_temp_c,     1);
    url += "&field2=";  url += String(g_gas_ppm,    0);
    url += "&field3=";  url += String(g_flame ? 1 : 0);
    url += "&field4=";  url += String(g_fuzzy_score,1);
    url += "&field5=";  url += String((int)g_risk_level);
    url += "&field6=";  url += String((int)g_gas_trend);

    Serial.println("[IoT] URL: " + url);  // Print full URL for debug

    WiFiClientSecure client;
    client.setInsecure();  // Skip SSL cert verification

    HTTPClient https;
    https.begin(client, url);
    https.setTimeout(8000);
    int code = https.GET();
    String body = https.getString();
    https.end();

    if (code == 200) {
        if (body == "0") {
            Serial.println("[IoT] ThingSpeak: Duplicate/No update (returned 0)");
        } else {
            Serial.print("[IoT] ThingSpeak: Entry #"); Serial.println(body);
        }
    } else {
        Serial.print("[IoT] ThingSpeak FAILED. HTTP="); Serial.print(code);
        Serial.print(" Body="); Serial.println(body);
    }
}

// ===== UPDATE =====
void iot_update() {
    unsigned long now = millis();

    // Auto-reconnect every 30s if disconnected
    if (!iot_connected()) {
        if (now - last_reconnect_ms > 30000) {
            last_reconnect_ms = now;
            wifi_connect();
        }
        return;
    }

    // Upload every 15 seconds
    if (now - last_upload_ms >= IOT_UPLOAD_INTERVAL) {
        last_upload_ms = now;
        thingspeak_upload();
    }
}
