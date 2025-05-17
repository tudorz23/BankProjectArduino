#include "debounce.h"
#include "pins.h"


/* STATIC VARIABLES (visible only inside this file) */

// For joystick delay between levels.
static unsigned long last_joy_delay_time = 0;

// For buttons debouncing.
static unsigned long last_joy_debounce_time = 0;
static unsigned long last_red_debounce_time = 0;

static int last_joy_button_state = HIGH;
static int joy_button_stable_state = HIGH;
static int last_red_button_state = HIGH;
static int red_button_stable_state = HIGH;


/*=====================================================================================*/
/* FUNCTION DEFINITIONS */

bool joystick_to_the_right() {
    int x_val = analogRead(JOYSTICK_VRX_PIN);

    if (x_val > JOY_RIGHT_THRESHOLD) {
        unsigned long curr_time = millis();
        if (curr_time - last_joy_delay_time > BETWEEN_MENUS_DELAY) {
            last_joy_delay_time = curr_time;
            return true;
        }
    }

    return false;
}


bool joystick_to_the_left() {
    int x_val = analogRead(JOYSTICK_VRX_PIN);

    if (x_val < JOY_LEFT_THRESHOLD) {
        unsigned long curr_time = millis();
        if (curr_time - last_joy_delay_time > BETWEEN_MENUS_DELAY) {
            last_joy_delay_time = curr_time;
            return true;
        }
    }

    return false;
}


bool joystick_to_up() {
    int y_val = analogRead(JOYSTICK_VRY_PIN);

    if (y_val < JOY_UP_THRESHOLD) {
        unsigned long curr_time = millis();
        if (curr_time - last_joy_delay_time > BETWEEN_MENUS_DELAY) {
            last_joy_delay_time = curr_time;
            return true;
        }
    }

    return false;
}


bool joystick_to_down() {
    int y_val = analogRead(JOYSTICK_VRY_PIN);

    if (y_val > JOY_DOWN_THRESHOLD) {
        unsigned long curr_time = millis();
        if (curr_time - last_joy_delay_time > BETWEEN_MENUS_DELAY) {
            last_joy_delay_time = curr_time;
            return true;
        }
    }

    return false;
}


bool is_button_pressed(const int pin, int &last_state, int &stable_state,
                       unsigned long &last_debounce_time) {
    int reading = digitalRead(pin);

    if (reading != last_state) {
        last_debounce_time = millis();
    }

    last_state = reading;

    if ((millis() - last_debounce_time) > DEBOUNCE_DELAY) {
        // Check if the button state has changed (since the last stable state)
        if (reading != stable_state) {
            stable_state = reading;

            if (stable_state == LOW) {
                return true;
            }
        }
    }

    return false;
}


bool is_joy_button_pressed() {
    return is_button_pressed(JOYSTICK_SW_PIN, last_joy_button_state,
                             joy_button_stable_state, last_joy_debounce_time);
}


bool is_red_button_pressed() {
    return is_button_pressed(RED_BUTTON_PIN, last_red_button_state,
                             red_button_stable_state, last_red_debounce_time);
}
