#include "menus.h"
#include "utils.h"


void MENU_error() {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(F("ERROR"));

    while (true) {
        if (is_button_pressed(RED_BUTTON_PIN, last_red_button_state,
                              red_button_stable_state, last_red_debounce_time)) {
            curr_menu = MENU_START_HELLO;
            return;
        }
    }
}


/*=====================================================================================*/
/* START menus */

void MENU_START_hello() {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(F("Welcome!"));

    while (true) {
        if (joystick_to_the_right()) {
            curr_menu = MENU_START_LOGIN;
            return;
        }
    }
}


void MENU_START_login() {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(F("Login"));

    while (true) {
        if (joystick_to_the_left()) {
            curr_menu = MENU_START_HELLO;
            return;
        }

        if (joystick_to_the_right()) {
            curr_menu = MENU_START_REGISTER;
            return;
        }

        // If Joystick button is pressed, access the LOGIN menu.
        if (is_button_pressed(JOYSTICK_SW_PIN, last_joy_button_state,
                              joy_button_stable_state, last_joy_debounce_time)) {
            curr_menu = MENU_LOGIN_SCAN;
            return;
        }
    }
}


void MENU_START_register() {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(F("Register"));

    while (true) {
        if (joystick_to_the_left()) {
            curr_menu = MENU_START_LOGIN;
            return;
        }

        // If Joystick button is pressed, access the REGISTER menu.
        if (is_button_pressed(JOYSTICK_SW_PIN, last_joy_button_state,
                              joy_button_stable_state, last_joy_debounce_time)) {
            curr_menu = MENU_REGISTER_SCAN;
            return;
        }
    }
}


/*=====================================================================================*/
/* REGISTER menus */

void MENU_REGISTER_scan() {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(F("Scan card"));

    while (true) {
        // If red button is pressed, abort scanning and go back to START menu.
        if (is_button_pressed(RED_BUTTON_PIN, last_red_button_state,
                              red_button_stable_state, last_red_debounce_time)) {
            curr_menu = MENU_START_REGISTER;
            return;
        }

        // Poll for a new card.
        if (!mfrc522.PICC_IsNewCardPresent()) {
            continue;
        }

        if (!mfrc522.PICC_ReadCardSerial()) {
            continue;
        }

        // Got a new card, read the UID.
        char uid[UID_SIZE + 1];
        extract_uid(uid);

        #ifdef DEBUG
        Serial.println(uid);
        #endif

        logged_user = get_user_idx_from_uid(uid);
        if (logged_user == NO_USER) {
            curr_menu = MENU_ERROR;
            return;
        }

        #ifdef DEBUG
        Serial.println(logged_user);
        if (logged_user != NO_USER) {
            Serial.println(names[logged_user]);
        }
        Serial.println();
        #endif

        curr_menu = MENU_REGISTER_PIN;
        return;
    }
}


void MENU_REGISTER_pin() {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(F("Enter PIN:"));

    uint16_t pin = get_pin_input();

    if (pin == 0) {
        // Red button was pressed.
        curr_menu = MENU_REGISTER_SCAN;
        return;
    }

    // PIN introduced successfuly.
    register_user(logged_user);

    users[logged_user].checking_sum = 0;
    users[logged_user].economy_sum = 0;
    users[logged_user].pin = pin;

    #ifdef DEBUG
    Serial.println(pin);
    #endif

    curr_menu = MENU_LOGGED_HELLO;
}


/*=====================================================================================*/
/* LOGIN MENUS */

void MENU_LOGIN_scan() {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(F("Scan card"));

    while (true) {
        // If red button is pressed, abort scanning and go back to START menu.
        if (is_button_pressed(RED_BUTTON_PIN, last_red_button_state,
                              red_button_stable_state, last_red_debounce_time)) {
            curr_menu = MENU_START_LOGIN;
            return;
        }

        // Poll for a new card.
        if (!mfrc522.PICC_IsNewCardPresent()) {
            continue;
        }

        if (!mfrc522.PICC_ReadCardSerial()) {
            continue;
        }

        // Got a new card, read the UID.
        char uid[UID_SIZE + 1];
        extract_uid(uid);

        #ifdef DEBUG
        Serial.println(uid);
        #endif

        logged_user = get_user_idx_from_uid(uid);
        if (logged_user == NO_USER) {
            curr_menu = MENU_ERROR;
            return;
        }

        #ifdef DEBUG
        Serial.println(logged_user);
        if (logged_user != NO_USER) {
            Serial.println(names[logged_user]);
        }
        Serial.println();
        #endif

        // Check if the user is registered.
        if (!is_user_registered(logged_user)) {
            logged_user = NO_USER;
            curr_menu = MENU_LOGIN_NOT_REGISTERED;
            return;
        }

        curr_menu = MENU_LOGIN_ENTER_PIN;
        return;
    }
}


void MENU_LOGIN_not_registered() {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(F("Not registered"));

    while (true) {
        // If red button is pressed, go back to LOGIN SCAN menu.
        if (is_button_pressed(RED_BUTTON_PIN, last_red_button_state,
                              red_button_stable_state, last_red_debounce_time)) {
            curr_menu = MENU_LOGIN_SCAN;
            return;
        }
    }
}


void MENU_LOGIN_enter_pin() {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(F("Enter PIN:"));

    uint16_t pin = get_pin_input();

    if (pin == 0) {
        // Red button was pressed.
        logged_user = NO_USER;
        curr_menu = MENU_LOGIN_SCAN;
        return;
    }

    // Check if the PIN is correct.
    if (pin != users[logged_user].pin) {
        curr_menu = MENU_LOGIN_WRONG_PIN;
        return;
    }

    curr_menu = MENU_LOGGED_HELLO;
}


void MENU_LOGIN_wrong_pin() {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(F("Wrong PIN"));

    while (true) {
        // If red button is pressed, abort and go back to LOGIN_ENTER_PIN menu.
        if (is_button_pressed(RED_BUTTON_PIN, last_red_button_state,
                              red_button_stable_state, last_red_debounce_time)) {
            curr_menu = MENU_LOGIN_ENTER_PIN;
            return;
        }
    }
}


/*=====================================================================================*/
/* LOGGED menus */

void MENU_LOGGED_hello() {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(F("Hello"));

    lcd.setCursor(0, 1);
    lcd.print(names[logged_user]);

    while (true) {
        // If red button is pressed, got to LOGOUT menu.
        if (is_button_pressed(RED_BUTTON_PIN, last_red_button_state,
                              red_button_stable_state, last_red_debounce_time)) {
            curr_menu = MENU_LOGGED_LOGOUT;
            return;
        }
    }
}


void MENU_LOGGED_logout() {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(F("Surely logout?"));

    while (true) {
        // If the joystick button is pressed, confirm the logout.
        if (is_button_pressed(JOYSTICK_SW_PIN, last_joy_button_state,
                              joy_button_stable_state, last_joy_debounce_time)) {
            logged_user = -1;
            curr_menu = MENU_START_HELLO;
            return;
        }

        // If the red button is pressed, cancel the logout.
        if (is_button_pressed(RED_BUTTON_PIN, last_red_button_state,
                              red_button_stable_state, last_red_debounce_time)) {
            curr_menu = MENU_LOGGED_HELLO;
            return;
        }
    }
}
