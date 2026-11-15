# AI-Based Smart Fire and Gas Leakage Detection System
### ESP32 Firmware — PlatformIO Project

**Platform:** ESP32 | **IDE:** PlatformIO (VS Code) or Arduino IDE  
**Sensors:** MQ2, Flame Sensor, Temperature Sensor  
**IoT:** ThingSpeak / Blynk | **Output:** LCD, LED, Buzzer  
**AI Approach:** Fuzzy/Weighted Scoring + Trend Analysis

---

## Folder Structure — Complete Explanation

```
E_Project/
├── platformio.ini
├── include/
│   └── globals.h
├── src/
│   ├── main.cpp
│   ├── sensors/
│   │   ├── mq2_sensor.h / .cpp
│   │   ├── flame_sensor.h / .cpp
│   │   └── temp_sensor.h / .cpp
│   ├── detection/
│   │   ├── fuzzy_classifier.h / .cpp
│   │   └── risk_classifier.h / .cpp
│   ├── trend_analysis/
│   │   └── trend_engine.h / .cpp
│   ├── alerts/
│   │   └── alert_manager.h / .cpp
│   ├── iot/
│   │   └── iot_manager.h / .cpp
│   ├── ui/                          ← NEW: OLED + Buttons UI
│   │   ├── oled_display.h / .cpp
│   │   ├── menu_manager.h / .cpp
│   │   └── button_handler.h / .cpp
│   └── utils/
│       └── config.h
├── lib/
├── data/
└── hardware/
```

---

## Har File / Folder ka Kaam

### platformio.ini
> PlatformIO ka config file hai.
> Isme ESP32 board ka naam, libraries (WiFi, MQTT, etc.), aur upload settings hoti hain.
> Jab aap "Upload" button dabate ho — yeh file batati hai kahan aur kaise upload karna hai.

---

### include/globals.h
> Poore project mein jo variables sab jagah use honge wo yahan declare hote hain.
> Maslan: current risk level, sensor readings — jo ek file se doosri file tak share ho.

---

### src/main.cpp ← SABSE IMPORTANT
> Yeh ESP32 ka DIL hai.
> `setup()` — ESP32 on hone par ek baar chalta hai (sensors initialize karo, WiFi connect karo).
> `loop()` — baar baar chalta hai (sensor padho → classify karo → alert do → IoT ko bhejo).

---

### src/sensors/ ← STEP 1: Data Collect Karo
> Teeno sensors ki reading yahan se aati hai.

| File | Kaam |
|---|---|
| `mq2_sensor` | MQ2 gas sensor se gas level padho (PPM mein) |
| `flame_sensor` | Flame sensor se fire detect karo (HIGH/LOW) |
| `temp_sensor` | Temperature padho (DHT11 ya DS18B20 se) |

---

### src/detection/ ← STEP 2: AI Decision Karo
> Sensor ki readings lo aur decide karo — SAFE hai, WARNING hai, ya DANGER hai.

| File | Kaam |
|---|---|
| `fuzzy_classifier` | 🤖 AI wala hissa — teeno sensors ko weight de ke Risk Score nikalo |
| `risk_classifier` | Risk Score dekh ke final label lagao: SAFE / WARNING / DANGER |

**Example:**
```
Gas=60%,  Flame=0%,  Temp=40%
Score = (60×0.5) + (0×0.3) + (40×0.2) = 30+0+8 = 38 → WARNING ⚠️
```

---

### src/trend_analysis/ ← STEP 3: Future Predict Karo
> Sensors ki values kitni tezi se badh rahi hain — yeh calculate karo.
> Agar gas abhi SAFE zone mein hai lekin tezi se upar ja rahi hai — PEHLE SE alert karo.

| File | Kaam |
|---|---|
| `trend_engine` | Har sensor ki rate-of-change nikalo aur early warning do |

**Example:**
```
Gas:  200 → 350 → 500 → 650  (har 2 sec mein +150)
→ "Abhi WARNING hai, lekin 4 sec mein DANGER hoga!" 🔴
```

---

### src/alerts/ ← STEP 4: Output Do
> Classification aur trend ka result lo — aur user ko batao.

| File | Kaam |
|---|---|
| `alert_manager` | LCD pe message dikhao, LED jalao, Buzzer bajao |

**Example:**
```
SAFE    → Green LED on,  Buzzer off, LCD: "System Normal"
WARNING → Yellow LED on, Buzzer beep, LCD: "Warning! Check Gas"
DANGER  → Red LED on,   Buzzer alarm, LCD: "DANGER! Evacuate!"
```

---

### src/iot/ ← STEP 5: Cloud Ko Bhejo
> Sensor data aur risk level ThingSpeak ya Blynk pe bhejo — taake remote monitoring ho sake.

| File | Kaam |
|---|---|
| `iot_manager` | WiFi se connect karo, data cloud pe upload karo, graphs banao |

---

### src/utils/config.h ← Settings File
> Poore project ki settings ek jagah — pins, thresholds, WiFi password sab yahan.

```cpp
// Pin numbers
#define MQ2_PIN       34
#define FLAME_PIN     35
#define TEMP_PIN      32
#define BUZZER_PIN    25
#define LED_RED       26

// Risk thresholds
#define DANGER_SCORE  70
#define WARNING_SCORE 40

// WiFi
#define WIFI_SSID     "YourWiFi"
#define WIFI_PASS     "YourPassword"
```

---

### lib/ ← External Libraries
> Jo libraries Arduino Library Manager se download hongi wo yahan aati hain automatically.
> (DHT library, LiquidCrystal, ThingSpeak, etc.)

---

### data/ ← ESP32 Flash Storage
> Agar koi file ESP32 ki internal memory (SPIFFS) mein store karni ho to yahan rakho.
> (Abhi ke liye khali hai)

---

### hardware/ ← Reference Only (Upload Nahi Hoga)
> Circuit diagrams, sensor datasheets, component list — sirf dekhne ke liye.
> Yeh ESP32 pe upload nahi hota.

---

## System Flow (Poora Process)

```
[4 Buttons: UP / DOWN / SELECT / BACK]
      ↓
[button_handler → Press detect karo]
      ↓
[menu_manager → Kaunsa screen dikhana hai?]
      ↓
      ├── Main Menu      → "Set Threshold" / "View Sensors"
      ├── Threshold Menu → UP/DOWN se value adjust, SELECT se save
      └── View Sensors   → Real-time MQ2, Flame, Temp values (Main Menu ka option 2)

[Sensors padho] ←──────────────────────────────────────┐
      ↓                                                  │
[fuzzy_classifier → Weighted Risk Score nikalo]          │
      ↓                                                  │
[trend_engine → Future predict karo]                     │
      ↓                                                  │
[risk_classifier → SAFE / WARNING / DANGER]              │
      ↓                                                  │
     /|\                                                 │
    / | \                                                │
[alerts][iot][oled_display] ────────────────────────────┘
LED    Cloud  OLED Screen
Buzzer ThingSpeak "FIRE ALERT!" / "SMOKE DETECTED!"
       /Blynk    SELECT = Alarm mute, monitoring jari
```

---

### src/ui/ ← NEW: OLED Display + Button Menu System

> 0.96 inch OLED display aur 4 push buttons se ek simple menu-driven interface.
> User sensors ki readings dekh sakta hai, thresholds set kar sakta hai, aur alarms acknowledge kar sakta hai.

---

#### `button_handler.h / .cpp` ← 4 Buttons ka Control

> Charon buttons ki press detect karta hai.

| Button | Kaam |
|---|---|
| **UP** | Menu mein upar jana / threshold value badhana |
| **DOWN** | Menu mein neeche jana / threshold value ghatana |
| **SELECT** | Menu open karo / settings save karo / alarm mute karo |
| **BACK** | Pichle screen pe wapas jao |

---

#### `menu_manager.h / .cpp` ← Menu Logic

> Kaunsa screen dikhana hai — yeh decide karta hai.
> 3 screens handle karta hai:

```
MAIN MENU
├── 1. Set Threshold   → SELECT dabao
│       ├── Temperature Limit  (UP/DOWN se set, SELECT se save)
│       └── Smoke (Gas) Limit  (UP/DOWN se set, SELECT se save)
│
└── 2. View Sensors    → SELECT dabao
        └── Real-time: Temp, Gas (PPM), Flame Status, Risk Level
```

---

#### `oled_display.h / .cpp` ← OLED Screen Draw karo

> Actual OLED pe text/graphics draw karta hai.

| Screen | Kya dikhta hai |
|---|---|
| **Main Menu** | "1. Set Threshold" / "2. View Sensors" |
| **Threshold Menu** | "Temp Limit: 55°C ↑↓" / "Gas Limit: 400 ↑↓" |
| **Sensor View** | "Temp: 42°C \| Gas: 320 \| Flame: NO \| SAFE ✓" |
| **FIRE ALERT!** | "🔴 FIRE ALERT! \| Press SELECT to mute" |
| **SMOKE DETECTED!** | "⚠ SMOKE DETECTED! \| SELECT = Mute" |

> **Alarm Acknowledge:** Emergency mein SELECT dabao → Buzzer band, OLED monitoring jari rakhta hai.
