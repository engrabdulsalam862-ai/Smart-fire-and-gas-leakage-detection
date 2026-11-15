#include "button_handler.h"
#include "../../src/utils/config.h"
#include "Arduino.h"

// =========================================================
//  Confirmed Debounce: press detected → wait DEBOUNCE_MS
//  → confirm still pressed → register ONCE → wait for release
// =========================================================
#define DEBOUNCE_MS     50
#define NUM_BUTTONS     4

static const uint8_t PINS[NUM_BUTTONS]      = { BTN_UP, BTN_DOWN, BTN_SELECT, BTN_BACK };
static const ButtonPress CODES[NUM_BUTTONS] = { BTN_UP_P, BTN_DOWN_P, BTN_SEL_P, BTN_BACK_P };

// Per-button state machine
enum BtnState { IDLE, DEBOUNCING, PRESSED };

static BtnState       btn_state[NUM_BUTTONS];
static unsigned long  btn_time[NUM_BUTTONS];

void button_init() {
    for (int i = 0; i < NUM_BUTTONS; i++) {
        pinMode(PINS[i], INPUT_PULLUP);
        btn_state[i] = IDLE;
        btn_time[i]  = 0;
    }
    Serial.println("[BTN] Initialized. Pins: UP=32 DN=33 SEL=14 BCK=12");
}

ButtonPress button_read() {
    unsigned long now = millis();

    for (int i = 0; i < NUM_BUTTONS; i++) {
        bool is_low = (digitalRead(PINS[i]) == LOW);  // LOW = pressed with INPUT_PULLUP

        switch (btn_state[i]) {

            case IDLE:
                if (is_low) {
                    // Button just went LOW — start debounce timer
                    btn_state[i] = DEBOUNCING;
                    btn_time[i]  = now;
                }
                break;

            case DEBOUNCING:
                if (!is_low) {
                    // Noise — went HIGH again before debounce finished
                    btn_state[i] = IDLE;
                }
                else if (now - btn_time[i] >= DEBOUNCE_MS) {
                    // Still LOW after debounce → confirmed press
                    btn_state[i] = PRESSED;
                    Serial.print("[BTN] Pressed: "); Serial.println(i);
                    return CODES[i];    // Return press event ONCE
                }
                break;

            case PRESSED:
                if (!is_low) {
                    // Button released → ready for next press
                    btn_state[i] = IDLE;
                }
                break;
        }
    }
    return BTN_NONE;
}
