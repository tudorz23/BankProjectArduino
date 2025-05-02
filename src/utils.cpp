#include "utils.h"


/* HELPER FUNCTIONS */
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


void extract_uid(char *buff) {
    // Read one byte (i.e. 2 hex chars) at a time.
    // Pad with one 0 to the left if the byte can be
    // written with only one hex char (i.e. curr_byte < 0x10).
    for (uint8_t i = 0; i < mfrc522.uid.size; i++) {
        sprintf(&buff[i * 2], "%02X", mfrc522.uid.uidByte[i]);
    }

    // Add null-terminator.
    buff[mfrc522.uid.size * 2] = '\0';
}


int8_t get_user_idx_from_uid(char *uid) {
    for (uint8_t i = 0; i < MAX_USERS; i++) {
        if (strcmp(uids[i], uid) == 0) {
            return i;
        }
    }

    return NO_USER;
}


uint32_t read_number_input(ReadInputType type) {
    // If a PIN is read, limit it to 4 digits, if a sum is read, limit it to MAX_SUM_DIGITS.
    uint8_t max_digits = type == PIN ? PIN_SIZE : MAX_SUM_DIGITS;

    uint32_t number = 0;
    uint8_t read_key = 0;
    uint8_t idx = 0;
    uint8_t last_key = 0;  // Keep track of last key pressed

    while (true) {
        // If red button is pressed ,return 0 to signal abort.
        if (is_button_pressed(RED_BUTTON_PIN, last_red_button_state,
                              red_button_stable_state, last_red_debounce_time)) {
            return 0;
        }

        // If joystick button is pressed.
        if (is_button_pressed(JOYSTICK_SW_PIN, last_joy_button_state,
                              joy_button_stable_state, last_joy_debounce_time)) {
            if (type == PIN && idx == PIN_SIZE) {
                return number;
            }

            if (type == SUM && idx > 0) {
                return number;
            }
        }

        // Read current key (non-blocking).
        read_key = ttp229.GetKey16();

        // Only react if no key was previously pressed (to avoid accidental multiple sampling).
        if (read_key != 0 && last_key == 0) {
            // '0' key
            if (read_key == 10) {
                if (idx != 0 && idx < max_digits) {
                    number = number * 10;
                    idx++;
                }
            }

            // Backspace
            else if (read_key == 11) {
                // Can't delete if there is no digit.
                if (idx != 0) {
                    number = number / 10;
                    idx--;
                }
            }

            // Normal digits
            else if (read_key <= 9) { 
                if (idx < max_digits) {
                    number = number * 10 + read_key;
                    idx++;
                }
            }

            // Print intermediary number
            lcd.setCursor(0, 1);
            lcd.print(BLANK); // clear
            lcd.setCursor(0, 1);
            if (number != 0) {
                lcd.print(number);
            }
        }

        // Update last_key
        last_key = read_key;
    }
}


void register_user(int8_t idx) {
    registered_users |= (1 << idx);
}


bool is_user_registered(int8_t idx) {
    return (registered_users & (1 << idx)) != 0;
}


void apply_interest(User &user) {
    // The wdt_counter is incremented every 1 second.
    uint16_t elapsed_time = wdt_counter - user.last_interest_update_time;

    #ifdef DEBUG
    Serial.println(user.last_interest_update_time);
    Serial.println(elapsed_time);
    #endif

    // The interest is applied every APPLY_INTEREST_INTERVAL seconds.
    uint8_t times_to_apply = elapsed_time / APPLY_INTEREST_INTERVAL;

    // Apply the interest times_to_apply times.
    for (uint8_t i = 0; i < times_to_apply; i++) {
        user.economy_sum += user.economy_sum * ((float)INTEREST_RATE / 100.0);
    }

    // Last update basically happened at last_time + times_to_apply * INTERVAL.
    user.last_interest_update_time += times_to_apply * APPLY_INTEREST_INTERVAL;

    #ifdef DEBUG
    Serial.println(user.last_interest_update_time);

    Serial.println();
    #endif
}
