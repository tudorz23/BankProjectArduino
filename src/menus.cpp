#include "menus.h"
#include "utils.h"

/* ERROR menu */
void MENU_error() {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(F("ERROR"));

    while (true) {
        if (is_button_pressed(RED_BUTTON_PIN, last_red_button_state,
                              red_button_stable_state, last_red_debounce_time)) {
            curr_menu = Menu::START_HELLO;
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
            curr_menu = Menu::START_LOGIN;
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
            curr_menu = Menu::START_HELLO;
            return;
        }

        if (joystick_to_the_right()) {
            curr_menu = Menu::START_REGISTER;
            return;
        }

        // If Joystick button is pressed, access the LOGIN menu.
        if (is_button_pressed(JOYSTICK_SW_PIN, last_joy_button_state,
                              joy_button_stable_state, last_joy_debounce_time)) {
            curr_menu = Menu::LOGIN_SCAN;
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
            curr_menu = Menu::START_LOGIN;
            return;
        }

        if (joystick_to_the_right()) {
            curr_menu = Menu::START_DEBUG;
            return;
        }

        // If Joystick button is pressed, access the REGISTER menu.
        if (is_button_pressed(JOYSTICK_SW_PIN, last_joy_button_state,
                              joy_button_stable_state, last_joy_debounce_time)) {
            curr_menu = Menu::REGISTER_SCAN;
            return;
        }
    }
}


void MENU_START_debug() {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(F("Debug"));

    while (true) {
        if (joystick_to_the_left()) {
            curr_menu = Menu::START_REGISTER;
            return;
        }

        // If Joystick button is pressed, access the DEBUG menu.
        if (is_button_pressed(JOYSTICK_SW_PIN, last_joy_button_state,
                              joy_button_stable_state, last_joy_debounce_time)) {
            curr_menu = Menu::DEBUG_WDT;
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
            curr_menu = Menu::START_REGISTER;
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
            curr_menu = Menu::ERROR;
            return;
        }

        if (is_user_registered(logged_user)) {
            // User already registered.
            logged_user = NO_USER;
            curr_menu = Menu::REGISTER_ALREADY_REG;
            return;
        }

        #ifdef DEBUG
        Serial.println(logged_user);
        Serial.println(names[logged_user]);
        Serial.println();
        #endif

        curr_menu = Menu::REGISTER_PIN;
        return;
    }
}


void MENU_REGISTER_already_reg() {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(F("Already"));
    lcd.setCursor(0, 1);
    lcd.print(F("registered"));

    while (true) {
        // If red button is pressed, go back to REGISTER SCAN menu.
        if (is_button_pressed(RED_BUTTON_PIN, last_red_button_state,
                              red_button_stable_state, last_red_debounce_time)) {
            curr_menu = Menu::REGISTER_SCAN;
            return;
        }
    }
}


void MENU_REGISTER_pin() {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(F("Enter PIN:"));

    // For safety reasons, read to a uint32_t
    uint32_t pin_big = read_number_input(ReadInputType::PIN);
    uint16_t pin = pin_big % 10000;

    if (pin == 0) {
        // Red button was pressed.
        logged_user = NO_USER;
        curr_menu = Menu::REGISTER_SCAN;
        return;
    }

    // PIN introduced successfuly.
    register_user(logged_user);

    users[logged_user].checking_sum = 200;
    users[logged_user].economy_sum = 100;
    users[logged_user].last_interest_update_time = wdt_counter;
    users[logged_user].pin = pin;

    #ifdef DEBUG
    Serial.println(pin);
    Serial.println();
    #endif

    curr_menu = Menu::LOGGED_HELLO;
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
            curr_menu = Menu::START_LOGIN;
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
            curr_menu = Menu::ERROR;
            return;
        }

        #ifdef DEBUG
        Serial.println(logged_user);
        Serial.println(names[logged_user]);
        Serial.println();
        #endif

        // Check if the user is registered.
        if (!is_user_registered(logged_user)) {
            logged_user = NO_USER;
            curr_menu = Menu::LOGIN_NOT_REGISTERED;
            return;
        }

        curr_menu = Menu::LOGIN_ENTER_PIN;
        return;
    }
}


void MENU_LOGIN_not_registered() {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(F("Not registered"));

    while (true) {
        // If red button is pressed, go back to LOGIN_SCAN menu.
        if (is_button_pressed(RED_BUTTON_PIN, last_red_button_state,
                              red_button_stable_state, last_red_debounce_time)) {
            curr_menu = Menu::LOGIN_SCAN;
            return;
        }
    }
}


void MENU_LOGIN_enter_pin() {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(F("Enter PIN:"));

    // For safety reasons, read to a uint32_t
    uint32_t pin_big = read_number_input(ReadInputType::PIN);
    uint16_t pin = pin_big % 10000;

    if (pin == 0) {
        // Red button was pressed.
        logged_user = NO_USER;
        curr_menu = Menu::LOGIN_SCAN;
        return;
    }

    // Check if the PIN is correct.
    if (pin != users[logged_user].pin) {
        curr_menu = Menu::LOGIN_WRONG_PIN;
        return;
    }

    curr_menu = Menu::LOGGED_HELLO;
}


void MENU_LOGIN_wrong_pin() {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(F("Wrong PIN"));

    while (true) {
        // If red button is pressed, go back to LOGIN_ENTER_PIN menu.
        if (is_button_pressed(RED_BUTTON_PIN, last_red_button_state,
                              red_button_stable_state, last_red_debounce_time)) {
            curr_menu = Menu::LOGIN_ENTER_PIN;
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
        if (joystick_to_the_right()) {
            curr_menu = Menu::LOGGED_MAIN_ACC;
            return;
        }

        // If red button is pressed, go to LOGOUT menu.
        if (is_button_pressed(RED_BUTTON_PIN, last_red_button_state,
                              red_button_stable_state, last_red_debounce_time)) {
            curr_menu = Menu::LOGGED_LOGOUT;
            return;
        }
    }
}


void MENU_LOGGED_main_acc() {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(F("Main acc"));

    while (true) {
        if (joystick_to_the_left()) {
            curr_menu = Menu::LOGGED_HELLO;
            return;
        }

        if (joystick_to_the_right()) {
            curr_menu = Menu::LOGGED_ECO_ACC;
            return;
        }

        // If Joystick button is pressed, access the MAIN_ACC menu.
        if (is_button_pressed(JOYSTICK_SW_PIN, last_joy_button_state,
                              joy_button_stable_state, last_joy_debounce_time)) {
            curr_menu = Menu::MAIN_ACC_SUM;
            return;
        }

        // If red button is pressed, go to LOGOUT menu.
        if (is_button_pressed(RED_BUTTON_PIN, last_red_button_state,
                              red_button_stable_state, last_red_debounce_time)) {
            curr_menu = Menu::LOGGED_LOGOUT;
            return;
        }
    }
}


void MENU_LOGGED_eco_acc() {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(F("Eco acc"));

    while (true) {
        if (joystick_to_the_left()) {
            curr_menu = Menu::LOGGED_MAIN_ACC;
            return;
        }

        // If Joystick button is pressed, access the ECO_ACC menu.
        if (is_button_pressed(JOYSTICK_SW_PIN, last_joy_button_state,
                              joy_button_stable_state, last_joy_debounce_time)) {
            curr_menu = Menu::ECO_ACC_SUM;
            return;
        }

        // If red button is pressed, go to LOGOUT menu.
        if (is_button_pressed(RED_BUTTON_PIN, last_red_button_state,
                              red_button_stable_state, last_red_debounce_time)) {
            curr_menu = Menu::LOGGED_LOGOUT;
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
            logged_user = NO_USER;
            curr_menu = Menu::START_HELLO;
            return;
        }

        // If the red button is pressed, cancel the logout.
        if (is_button_pressed(RED_BUTTON_PIN, last_red_button_state,
                              red_button_stable_state, last_red_debounce_time)) {
            curr_menu = Menu::LOGGED_HELLO;
            return;
        }
    }
}


/*=====================================================================================*/
/* MAIN_ACC menus */
void MENU_MAIN_ACC_sum() {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(F("Main acc sum:"));
    lcd.setCursor(0, 1);
    lcd.print(users[logged_user].checking_sum);

    while (true) {
        if (joystick_to_the_right()) {
            curr_menu = Menu::MAIN_ACC_ADD;
            return;
        }

        // If red button is pressed, go to LOGGED_MAIN_ACC menu.
        if (is_button_pressed(RED_BUTTON_PIN, last_red_button_state,
                              red_button_stable_state, last_red_debounce_time)) {
            curr_menu = Menu::LOGGED_MAIN_ACC;
            return;
        }
    }
}


void MENU_MAIN_ACC_add() {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(F("Add cash"));

    while (true) {
        if (joystick_to_the_left()) {
            curr_menu = Menu::MAIN_ACC_SUM;
            return;
        }

        if (joystick_to_the_right()) {
            curr_menu = Menu::MAIN_ACC_PAY;
            return;
        }

        // If the joystick button is pressed, go to ENTER_SUM.
        if (is_button_pressed(JOYSTICK_SW_PIN, last_joy_button_state,
                              joy_button_stable_state, last_joy_debounce_time)) {
            enter_sum_type = EnterSum::ADD_CASH;
            curr_menu = Menu::ENTER_SUM;
            return;
        }

        // If red button is pressed, go to LOGGED_MAIN_ACC menu.
        if (is_button_pressed(RED_BUTTON_PIN, last_red_button_state,
                              red_button_stable_state, last_red_debounce_time)) {
            curr_menu = Menu::LOGGED_MAIN_ACC;
            return;
        }
    }
}


void MENU_MAIN_ACC_pay() {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(F("Pay"));

    while (true) {
        if (joystick_to_the_left()) {
            curr_menu = Menu::MAIN_ACC_ADD;
            return;
        }

        if (joystick_to_the_right()) {
            curr_menu = Menu::MAIN_ACC_TO_ECO;
            return;
        }

        // If the joystick button is pressed, go to ENTER_SUM.
        if (is_button_pressed(JOYSTICK_SW_PIN, last_joy_button_state,
                              joy_button_stable_state, last_joy_debounce_time)) {
            enter_sum_type = EnterSum::PAY;
            curr_menu = Menu::ENTER_SUM;
            return;
        }

        // If red button is pressed, go to LOGGED_MAIN_ACC menu.
        if (is_button_pressed(RED_BUTTON_PIN, last_red_button_state,
                              red_button_stable_state, last_red_debounce_time)) {
            curr_menu = Menu::LOGGED_MAIN_ACC;
            return;
        }
    }
}


void MENU_MAIN_ACC_to_eco() {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(F("To Eco"));

    while (true) {
        if (joystick_to_the_left()) {
            curr_menu = Menu::MAIN_ACC_PAY;
            return;
        }

        // If the joystick button is pressed, go to ENTER_SUM.
        if (is_button_pressed(JOYSTICK_SW_PIN, last_joy_button_state,
                              joy_button_stable_state, last_joy_debounce_time)) {
            enter_sum_type = EnterSum::MAIN_TO_ECO;
            curr_menu = Menu::ENTER_SUM;
            return;
        }

        // If red button is pressed, go to LOGGED_MAIN_ACC menu.
        if (is_button_pressed(RED_BUTTON_PIN, last_red_button_state,
                              red_button_stable_state, last_red_debounce_time)) {
            curr_menu = Menu::LOGGED_MAIN_ACC;
            return;
        }
    }
}


/*=====================================================================================*/
/* ECO_ACC menus */
void MENU_ECO_ACC_sum() {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(F("Eco acc sum:"));
    lcd.setCursor(0, 1);

    // Add interest to the economy sum.
    apply_interest(users[logged_user]);
    lcd.print(users[logged_user].economy_sum);

    while (true) {
        if (joystick_to_the_right()) {
            curr_menu = Menu::ECO_ACC_TO_MAIN;
            return;
        }

        // If red button is pressed, go to LOGGED_ECO_ACC menu.
        if (is_button_pressed(RED_BUTTON_PIN, last_red_button_state,
                              red_button_stable_state, last_red_debounce_time)) {
            curr_menu = Menu::LOGGED_ECO_ACC;
            return;
        }
    }
}


void MENU_ECO_ACC_to_main() {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(F("To Main"));

    while (true) {
        if (joystick_to_the_left()) {
            curr_menu = Menu::ECO_ACC_SUM;
            return;
        }

        // If the joystick button is pressed, go to ENTER_SUM.
        if (is_button_pressed(JOYSTICK_SW_PIN, last_joy_button_state,
                              joy_button_stable_state, last_joy_debounce_time)) {
            enter_sum_type = EnterSum::ECO_TO_MAIN;
            curr_menu = Menu::ENTER_SUM;
            return;
        }

        // If red button is pressed, go to LOGGED_ECO_ACC menu.
        if (is_button_pressed(RED_BUTTON_PIN, last_red_button_state,
                              red_button_stable_state, last_red_debounce_time)) {
            curr_menu = Menu::LOGGED_ECO_ACC;
            return;
        }
    }
}


/*=====================================================================================*/
/* ENTER_SUM menu */
void MENU_ENTER_sum() {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(F("Enter sum:"));

    uint32_t sum = read_number_input(ReadInputType::SUM);

    if (sum == 0) {
        // Red button was pressed.
        if (enter_sum_type == EnterSum::ADD_CASH) curr_menu = Menu::MAIN_ACC_ADD;
        else if (enter_sum_type == EnterSum::MAIN_TO_ECO) curr_menu = Menu::MAIN_ACC_TO_ECO;
        else if (enter_sum_type == EnterSum::ECO_TO_MAIN) curr_menu = Menu::ECO_ACC_TO_MAIN;
        else if (enter_sum_type == EnterSum::PAY) curr_menu = Menu::MAIN_ACC_PAY;

        enter_sum_type = EnterSum::NO_ENTER;
        return;
    }

    if (enter_sum_type == EnterSum::ADD_CASH) {
        users[logged_user].checking_sum += sum;
        curr_menu = Menu::TRANSACTION_DONE;
    } else if (enter_sum_type == EnterSum::MAIN_TO_ECO) {
        if (users[logged_user].checking_sum >= sum) {
            users[logged_user].checking_sum -= sum;
            users[logged_user].economy_sum += sum;
            curr_menu = Menu::TRANSACTION_DONE;
        } else {
            curr_menu = Menu::NO_FUNDS;
        }
    } else if (enter_sum_type == EnterSum::ECO_TO_MAIN) {
        apply_interest(users[logged_user]);

        if (users[logged_user].economy_sum >= (float)sum) {
            users[logged_user].economy_sum -= sum;
            users[logged_user].checking_sum += sum;
            curr_menu = Menu::TRANSACTION_DONE;
        } else {
            curr_menu = Menu::NO_FUNDS;
        }
    } else {
        // enter_sum_type == PAY
        if (users[logged_user].checking_sum >= sum) {
            users[logged_user].checking_sum -= sum;
            curr_menu = Menu::TRANSACTION_DONE;
        } else {
            curr_menu = Menu::NO_FUNDS;
        }
    }
}


/*=====================================================================================*/
/* TRANSACTION_DONE menu */
void MENU_TRANSACTION_DONE_done() {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(F("Done"));

    while (true) {
        // If red button is pressed, go back to previous menu.
        if (is_button_pressed(RED_BUTTON_PIN, last_red_button_state,
                              red_button_stable_state, last_red_debounce_time)) {
            if (enter_sum_type == EnterSum::ADD_CASH) curr_menu = Menu::MAIN_ACC_SUM;
            else if (enter_sum_type == EnterSum::MAIN_TO_ECO) curr_menu = Menu::MAIN_ACC_SUM;
            else if (enter_sum_type == EnterSum::ECO_TO_MAIN) curr_menu = Menu::ECO_ACC_SUM;
            else if (enter_sum_type == EnterSum::PAY) curr_menu = Menu::MAIN_ACC_SUM;

            enter_sum_type = EnterSum::NO_ENTER;
            return;
        }
    }
}


/*=====================================================================================*/
/* NO_FUNDS menu */
void MENU_NO_funds() {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(F("No funds"));

    while (true) {
        // If red button is pressed, go back to previous menu.
        if (is_button_pressed(RED_BUTTON_PIN, last_red_button_state,
                              red_button_stable_state, last_red_debounce_time)) {
            if (enter_sum_type == EnterSum::ADD_CASH) curr_menu = Menu::MAIN_ACC_SUM;
            else if (enter_sum_type == EnterSum::MAIN_TO_ECO) curr_menu = Menu::MAIN_ACC_SUM;
            else if (enter_sum_type == EnterSum::ECO_TO_MAIN) curr_menu = Menu::ECO_ACC_SUM;
            else if (enter_sum_type == EnterSum::PAY) curr_menu = Menu::MAIN_ACC_SUM;

            enter_sum_type = EnterSum::NO_ENTER;
            return;
        }
    }
}


/*=====================================================================================*/
/* DEBUG menus */
void MENU_DEBUG_wdt() {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(F("WDT:"));

    lcd.setCursor(0, 1);
    lcd.print(wdt_counter);

    while (true) {
        // If the joystick button is pressed, reprint the counter.
        if (joystick_to_up()) {
            lcd.setCursor(0, 1);
            lcd.print(BLANK);
            lcd.setCursor(0, 1);
            lcd.print(wdt_counter);
        }

        // If the red button is pressed, return to START menu.
        if (is_button_pressed(RED_BUTTON_PIN, last_red_button_state,
                              red_button_stable_state, last_red_debounce_time)) {
            curr_menu = Menu::START_DEBUG;
            return;
        }
    }
}
