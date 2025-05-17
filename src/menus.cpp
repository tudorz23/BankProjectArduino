#include "menus.h"
#include "utils.h"
#include "debounce.h"
#include "sounds.h"
#include "wdt_counter.h"
#include "lights.h"

/* STATIC VARIABLES (private to this file) */

// Indicates to the Menu::ENTER_SUM what action to perform.
static EnterSum enter_sum_type = EnterSum::NO_ENTER;

// To know in the Menu::ENTER_SUM who is the target friend (when it is the case).
static int8_t friend_to_send_money = NO_USER;

// To know if the greeting message should be displayed in Menu::LOGGED_HELLO.
static bool refresh_logged_hello = true;


/*=====================================================================================*/
/* ERROR menu */
void MENU_error() {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(F("ERROR"));

    set_color_red();
    sound_login_failed();

    while (true) {
        if (is_red_button_pressed()) {
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
    set_color_green();

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
    set_color_green();

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
        if (is_joy_button_pressed()) {
            curr_menu = Menu::LOGIN_SCAN;
            return;
        }
    }
}


void MENU_START_register() {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(F("Register"));
    set_color_green();

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
        if (is_joy_button_pressed()) {
            curr_menu = Menu::REGISTER_SCAN;
            return;
        }
    }
}


void MENU_START_debug() {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(F("Debug"));
    set_color_green();

    while (true) {
        if (joystick_to_the_left()) {
            curr_menu = Menu::START_REGISTER;
            return;
        }

        // If Joystick button is pressed, access the DEBUG menu.
        if (is_joy_button_pressed()) {
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
    set_color_purple();

    while (true) {
        // If red button is pressed, abort scanning and go back to START menu.
        if (is_red_button_pressed()) {
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

    set_color_red();
    sound_login_failed();

    while (true) {
        // If red button is pressed, go back to START_REGISTER menu.
        if (is_red_button_pressed()) {
            curr_menu = Menu::START_REGISTER;
            return;
        }
    }
}


void MENU_REGISTER_pin() {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(F("Enter PIN:"));
    set_color_purple();

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

    users[logged_user].checking_sum = INITIAL_CHECKING_SUM;
    users[logged_user].economy_sum = INITIAL_ECO_SUM;
    users[logged_user].last_interest_update_time = get_wdt_counter();
    users[logged_user].pin = pin;
    users[logged_user].notif_cnt = 0;

    curr_menu = Menu::LOGGED_HELLO;
    greet_logged_user();
    set_color_yellow();
    sound_login_successful();
    refresh_logged_hello = false;
}


/*=====================================================================================*/
/* LOGIN MENUS */
void MENU_LOGIN_scan() {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(F("Scan card"));
    set_color_blue();

    while (true) {
        // If red button is pressed, abort scanning and go back to START menu.
        if (is_red_button_pressed()) {
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

        logged_user = get_user_idx_from_uid(uid);
        if (logged_user == NO_USER) {
            curr_menu = Menu::ERROR;
            return;
        }

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

    set_color_red();
    sound_login_failed();

    while (true) {
        // If red button is pressed, go back to START_LOGIN menu.
        if (is_red_button_pressed()) {
            curr_menu = Menu::START_LOGIN;
            return;
        }
    }
}


void MENU_LOGIN_enter_pin() {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(F("Enter PIN:"));
    set_color_blue();

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
    greet_logged_user();
    set_color_yellow();
    sound_login_successful();
    refresh_logged_hello = false;
}


void MENU_LOGIN_wrong_pin() {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(F("Wrong PIN"));

    set_color_red();
    sound_login_failed();

    while (true) {
        // If red button is pressed, go back to LOGIN_ENTER_PIN menu.
        if (is_red_button_pressed()) {
            curr_menu = Menu::LOGIN_ENTER_PIN;
            return;
        }
    }
}


/*=====================================================================================*/
/* LOGGED menus */
void MENU_LOGGED_hello() {
    if (logged_user == NO_USER) {
        curr_menu = Menu::ERROR;
        return;
    }

    if (refresh_logged_hello) {
        set_color_yellow();
        greet_logged_user();
    } else {
        refresh_logged_hello = true;
    }
    
    while (true) {
        if (joystick_to_the_left()) {
            curr_menu = Menu::LOGGED_NOTIFS;
            return;
        }

        if (joystick_to_the_right()) {
            curr_menu = Menu::LOGGED_MAIN_ACC;
            return;
        }

        // If red button is pressed, go to LOGOUT menu.
        if (is_red_button_pressed()) {
            curr_menu = Menu::LOGGED_LOGOUT;
            return;
        }
    }
}


void MENU_LOGGED_main_acc() {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(F("Main account"));
    set_color_yellow();

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
        if (is_joy_button_pressed()) {
            curr_menu = Menu::MAIN_ACC_SUM;
            return;
        }

        // If red button is pressed, go to LOGOUT menu.
        if (is_red_button_pressed()) {
            curr_menu = Menu::LOGGED_LOGOUT;
            return;
        }
    }
}


void MENU_LOGGED_eco_acc() {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(F("Eco account"));
    set_color_yellow();

    while (true) {
        if (joystick_to_the_left()) {
            curr_menu = Menu::LOGGED_MAIN_ACC;
            return;
        }

        if (joystick_to_the_right()) {
            curr_menu = Menu::LOGGED_FRIENDS;
            return;
        }

        // If Joystick button is pressed, access the ECO_ACC menu.
        if (is_joy_button_pressed()) {
            curr_menu = Menu::ECO_ACC_SUM;
            return;
        }

        // If red button is pressed, go to LOGOUT menu.
        if (is_red_button_pressed()) {
            curr_menu = Menu::LOGGED_LOGOUT;
            return;
        }
    }
}


void MENU_LOGGED_logout() {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(F("Surely logout?"));
    set_color_red();

    while (true) {
        // If the joystick button is pressed, confirm the logout.
        if (is_joy_button_pressed()) {
            logged_user = NO_USER;
            curr_menu = Menu::START_HELLO;

            sound_logout();
            return;
        }

        // If the red button is pressed, cancel the logout.
        if (is_red_button_pressed()) {
            curr_menu = Menu::LOGGED_HELLO;
            return;
        }
    }
}


void MENU_LOGGED_friends() {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(F("Friends"));
    set_color_yellow();

    while (true) {
        if (joystick_to_the_left()) {
            curr_menu = Menu::LOGGED_ECO_ACC;
            return;
        }

        if (joystick_to_the_right()) {
            curr_menu = Menu::LOGGED_CHANGE_PIN;
            return;
        }

        // If Joystick button is pressed, access the FRIENDS menu.
        if (is_joy_button_pressed()) {
            curr_menu = Menu::FRIENDS_SEE;
            return;
        }

        // If red button is pressed, go to LOGOUT menu.
        if (is_red_button_pressed()) {
            curr_menu = Menu::LOGGED_LOGOUT;
            return;
        }
    }
}


void MENU_LOGGED_notifications() {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(F("Notifications"));
    lcd.setCursor(0, 1);
    lcd.print(users[logged_user].notif_cnt);
    lcd.setCursor(2, 1);
    lcd.print("waiting");
    set_color_yellow();

    if (users[logged_user].notif_cnt > 0) {
        sound_new_notification();
    }

    while (true) {
        if (joystick_to_the_right()) {
            curr_menu = Menu::LOGGED_HELLO;
            return;
        }

        // If Joystick button is pressed, access the NOTIFICATIONS menu.
        if (is_joy_button_pressed()) {
            if (users[logged_user].notif_cnt == 0) {
                curr_menu = Menu::NOTIFICATIONS_NO_NEW;
            } else {
                curr_menu = Menu::NOTIFICATIONS_SEE;
            }
            
            return;
        }

        // If red button is pressed, go to LOGOUT menu.
        if (is_red_button_pressed()) {
            curr_menu = Menu::LOGGED_LOGOUT;
            return;
        }
    }
}


void MENU_LOGGED_change_pin() {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(F("Change PIN"));
    set_color_yellow();

    while (true) {
        if (joystick_to_the_left()) {
            curr_menu = Menu::LOGGED_FRIENDS;
            return;
        }

        // If Joystick button is pressed, access the CHANGE_PIN menu.
        if (is_joy_button_pressed()) {
            curr_menu = Menu::CHANGE_PIN_ENTER;
            return;
        }

        // If red button is pressed, go to LOGOUT menu.
        if (is_red_button_pressed()) {
            curr_menu = Menu::LOGGED_LOGOUT;
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
    set_color_blue();

    while (true) {
        if (joystick_to_the_right()) {
            curr_menu = Menu::MAIN_ACC_ADD;
            return;
        }

        // If red button is pressed, go to LOGGED_MAIN_ACC menu.
        if (is_red_button_pressed()) {
            curr_menu = Menu::LOGGED_MAIN_ACC;
            return;
        }
    }
}


void MENU_MAIN_ACC_add() {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(F("Add cash"));
    set_color_blue();

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
        if (is_joy_button_pressed()) {
            enter_sum_type = EnterSum::ADD_CASH;
            curr_menu = Menu::ENTER_SUM;
            return;
        }

        // If red button is pressed, go to LOGGED_MAIN_ACC menu.
        if (is_red_button_pressed()) {
            curr_menu = Menu::LOGGED_MAIN_ACC;
            return;
        }
    }
}


void MENU_MAIN_ACC_pay() {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(F("Pay"));
    set_color_blue();

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
        if (is_joy_button_pressed()) {
            enter_sum_type = EnterSum::PAY;
            curr_menu = Menu::ENTER_SUM;
            return;
        }

        // If red button is pressed, go to LOGGED_MAIN_ACC menu.
        if (is_red_button_pressed()) {
            curr_menu = Menu::LOGGED_MAIN_ACC;
            return;
        }
    }
}


void MENU_MAIN_ACC_to_eco() {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(F("To Eco"));
    set_color_blue();

    while (true) {
        if (joystick_to_the_left()) {
            curr_menu = Menu::MAIN_ACC_PAY;
            return;
        }

        if (joystick_to_the_right()) {
            curr_menu = Menu::MAIN_ACC_SEND_FRIEND;
            return;
        }

        // If the joystick button is pressed, go to ENTER_SUM.
        if (is_joy_button_pressed()) {
            enter_sum_type = EnterSum::MAIN_TO_ECO;
            curr_menu = Menu::ENTER_SUM;
            return;
        }

        // If red button is pressed, go to LOGGED_MAIN_ACC menu.
        if (is_red_button_pressed()) {
            curr_menu = Menu::LOGGED_MAIN_ACC;
            return;
        }
    }
}


void MENU_MAIN_ACC_send_friend() {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(F("Send to friend"));
    set_color_blue();

    while (true) {
        if (joystick_to_the_left()) {
            curr_menu = Menu::MAIN_ACC_TO_ECO;
            return;
        }

        // If the joystick button is pressed, go to SEND_FRIEND_choose.
        if (is_joy_button_pressed()) {
            curr_menu = Menu::SEND_FRIEND_CHOOSE;
            return;
        }

        // If red button is pressed, go to LOGGED_MAIN_ACC menu.
        if (is_red_button_pressed()) {
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
    set_color_green();

    // Add interest to the economy sum.
    apply_interest(users[logged_user]);
    lcd.print(users[logged_user].economy_sum);

    while (true) {
        if (joystick_to_the_right()) {
            curr_menu = Menu::ECO_ACC_TO_MAIN;
            return;
        }

        // If red button is pressed, go to LOGGED_ECO_ACC menu.
        if (is_red_button_pressed()) {
            curr_menu = Menu::LOGGED_ECO_ACC;
            return;
        }
    }
}


void MENU_ECO_ACC_to_main() {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(F("To Main"));
    set_color_green();

    while (true) {
        if (joystick_to_the_left()) {
            curr_menu = Menu::ECO_ACC_SUM;
            return;
        }

        // If the joystick button is pressed, go to ENTER_SUM.
        if (is_joy_button_pressed()) {
            enter_sum_type = EnterSum::ECO_TO_MAIN;
            curr_menu = Menu::ENTER_SUM;
            return;
        }

        // If red button is pressed, go to LOGGED_ECO_ACC menu.
        if (is_red_button_pressed()) {
            curr_menu = Menu::LOGGED_ECO_ACC;
            return;
        }
    }
}


/*=====================================================================================*/
/* SEND_FRIEND menus */
void MENU_SEND_FRIEND_choose() {
    lcd.clear();
    lcd.setCursor(0, 0);

    int8_t curr_friend = get_first_friend(logged_user);
    if (curr_friend == -1) {
        // No friend found.
        curr_menu = Menu::SEND_FRIEND_NO_FRIEND;
        return;
    }

    lcd.print(users[curr_friend].name);
    set_color_purple();

    while (true) {
        if (joystick_to_the_left()) {
            curr_friend = get_prev_friend(logged_user, curr_friend);
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print(users[curr_friend].name);
        }

        if (joystick_to_the_right()) {
            curr_friend = get_next_friend(logged_user, curr_friend);
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print(users[curr_friend].name);
        }

        // If the joystick button is pressed, go to ENTER_SUM.
        if (is_joy_button_pressed()) {
            friend_to_send_money = curr_friend;
            enter_sum_type = EnterSum::SEND_FRIEND;
            curr_menu = Menu::ENTER_SUM;
            return;
        }

        // If red button is pressed, go to MAIN_ACC_SEND_FRIEND menu.
        if (is_red_button_pressed()) {
            curr_menu = Menu::MAIN_ACC_SEND_FRIEND;
            return;
        }
    }
}


void MENU_SEND_FRIEND_no_friend() {
    lcd.print(F("You have"));
    lcd.setCursor(0, 1);
    lcd.print(F("no friends"));
    set_color_red();

    while (true) {
        // If red button is pressed, go to MAIN_ACC_SEND_FRIEND menu.
        if (is_red_button_pressed()) {
            curr_menu = Menu::MAIN_ACC_SEND_FRIEND;
            return;
        }
    }
}


/*=====================================================================================*/
/* FRIENDS menus */
void MENU_FRIENDS_see() {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(F("See friends"));
    set_color_yellow();

    while (true) {
        if (joystick_to_the_right()) {
            curr_menu = Menu::FRIENDS_ADD;
            return;
        }

        // If the joystick button is pressed, go to VIEW_FRIENDS menu.
        if (is_joy_button_pressed()) {
            curr_menu = Menu::VIEW_FRIENDS;
            return;
        }

        // If red button is pressed, go to LOGGED_FRIENDS menu.
        if (is_red_button_pressed()) {
            curr_menu = Menu::LOGGED_FRIENDS;
            return;
        }
    }
}


void MENU_FRIENDS_add() {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(F("Add friends"));
    set_color_yellow();

    while (true) {
        if (joystick_to_the_left()) {
            curr_menu = Menu::FRIENDS_SEE;
            return;
        }

        // If the joystick button is pressed, go to ADD_FRIENDS menu.
        if (is_joy_button_pressed()) {
            curr_menu = Menu::ADD_FRIENDS;
            return;
        }

        // If red button is pressed, go to LOGGED_FRIENDS menu.
        if (is_red_button_pressed()) {
            curr_menu = Menu::LOGGED_FRIENDS;
            return;
        }
    }
}


/*=====================================================================================*/
/* VIEW_FRIENDS menus */
void MENU_VIEW_FRIENDS_see() {
    lcd.clear();
    lcd.setCursor(0, 0);

    int8_t curr_friend = get_first_friend(logged_user);
    if (curr_friend == -1) {
        // No friend found.
        curr_menu = Menu::VIEW_FRIENDS_NO_FRIEND;
        return;
    }

    lcd.print(users[curr_friend].name);
    set_color_yellow();

    while (true) {
        if (joystick_to_the_left()) {
            curr_friend = get_prev_friend(logged_user, curr_friend);
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print(users[curr_friend].name);
        }

        if (joystick_to_the_right()) {
            curr_friend = get_next_friend(logged_user, curr_friend);
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print(users[curr_friend].name);
        }

        // If red button is pressed, go to FRIENDS_see menu.
        if (is_red_button_pressed()) {
            curr_menu = Menu::FRIENDS_SEE;
            return;
        }
    }
}


void MENU_VIEW_FRIENDS_no_friend() {
    lcd.print(F("You have"));
    lcd.setCursor(0, 1);
    lcd.print(F("no friends"));
    set_color_red();

    while (true) {
        // If red button is pressed, go to FRIENDS_see menu.
        if (is_red_button_pressed()) {
            curr_menu = Menu::FRIENDS_SEE;
            return;
        }
    }
}


/*=====================================================================================*/
/* ADD_FRIENDS menus */
void MENU_ADD_FRIENDS_add() {
    lcd.clear();
    lcd.setCursor(0, 0);

    int8_t curr_candidate = get_first_friend_candidate(logged_user);
    if (curr_candidate == -1) {
        // No candidate found.
        curr_menu = Menu::ADD_FRIENDS_NO_CANDIDATE;
        return;
    }

    lcd.print(users[curr_candidate].name);
    set_color_yellow();

    bool added_a_friend = false;

    while (true) {
        if (!added_a_friend) {
            // Menu where the friend candidates are shown.

            if (joystick_to_the_left()) {
                curr_candidate = get_prev_friend_candidate(logged_user, curr_candidate);
                lcd.clear();
                lcd.setCursor(0, 0);
                lcd.print(users[curr_candidate].name);
            }

            if (joystick_to_the_right()) {
                curr_candidate = get_next_friend_candidate(logged_user, curr_candidate);
                lcd.clear();
                lcd.setCursor(0, 0);
                lcd.print(users[curr_candidate].name);
            }

            // If the joystick button is pressed, send friend request and set added_a_friend to true.
            if (is_joy_button_pressed()) {
                // Send friend request.
                add_notification_to_inbox(curr_candidate, logged_user, NotifType::FriendReq, 0);

                // Mark friend req from user to candidate as sent.
                sent_friend_req[logged_user][curr_candidate] = true;

                added_a_friend = true;

                lcd.setCursor(0, 1);
                lcd.print("Friend req sent");
                set_color_green();
                sound_accept();
                continue;
            }

            // If red button is pressed, go to FRIENDS_add menu.
            if (is_red_button_pressed()) {
                curr_menu = Menu::FRIENDS_ADD;
                return;
            }
        }

        else {
            // Menu after a candidate was clicked.

            // If red button is pressed, start showing other friend candidates.
            if (is_red_button_pressed()) {
                // Try to get another candidate.
                curr_candidate = get_first_friend_candidate(logged_user);
                if (curr_candidate == -1) {
                    // No candidate found.
                    curr_menu = Menu::ADD_FRIENDS_NO_CANDIDATE;
                    return;
                }

                lcd.clear();
                lcd.setCursor(0, 0);
                lcd.print(users[curr_candidate].name);
                set_color_yellow();

                added_a_friend = false;
            }
        }
    }
}


void MENU_ADD_FRIENDS_no_candidate() {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(F("Noone to"));
    lcd.setCursor(0, 1);
    lcd.print(F("befriend"));
    set_color_red();

    while (true) {
        // If red button is pressed, go to FRIENDS_add menu.
        if (is_red_button_pressed()) {
            curr_menu = Menu::FRIENDS_ADD;
            return;
        }
    }
}


/*=====================================================================================*/
/* NOTIFICATIONS menus */
void MENU_NOTIFICATIONS_see() {
    if (users[logged_user].notif_cnt == 0) {
        // No new notifications
        curr_menu = Menu::NOTIFICATIONS_NO_NEW;
        return;
    }

    // Display first notification.
    int8_t curr_notif = 0;
    display_notification(users[logged_user].notifications[curr_notif]);
    set_color_purple();

    // Start in the view mode.
    NotifMode mode = NotifMode::VIEW;

    while (true) {
        if (mode == NotifMode::VIEW) {
            // Menu for iterating notifications.

            if (joystick_to_the_left()) {
                curr_notif = get_prev_notification(curr_notif);
                display_notification(users[logged_user].notifications[curr_notif]);
            }

            if (joystick_to_the_right()) {
                curr_notif = get_next_notification(users[logged_user].notif_cnt, curr_notif);
                display_notification(users[logged_user].notifications[curr_notif]);
            }

            if (joystick_to_up()) {
                // If the notif is FriendReq, accept the friend request.
                Notification &notif = users[logged_user].notifications[curr_notif];
                if (notif.type == NotifType::FriendReq) {
                    friendships[logged_user][notif.from_who] = true;
                    friendships[notif.from_who][logged_user] = true;

                    // Send notif to the sender that his req was accepted.
                    add_notification_to_inbox(notif.from_who, logged_user, NotifType::ReqAccepted, 0);

                    // Mark friend req from who to user as not sent anymore.
                    sent_friend_req[notif.from_who][logged_user] = false;

                    lcd.clear();
                    lcd.setCursor(0, 0);
                    lcd.print(users[notif.from_who].name);
                    lcd.setCursor(0, 1);
                    lcd.print(F("is now a friend"));

                    mode = NotifMode::ACCEPTED;
                    set_color_green();
                    sound_accept();
                    continue;
                }
            }

            if (joystick_to_down()) {
                // If the notif is FriendReq, reject the friend request.
                Notification &notif = users[logged_user].notifications[curr_notif];
                if (notif.type == NotifType::FriendReq) {

                    // Mark friend req from who to user as not sent anymore.
                    sent_friend_req[notif.from_who][logged_user] = false;

                    lcd.clear();
                    lcd.setCursor(0, 0);
                    lcd.print(F("Req rejected"));

                    mode = NotifMode::REJECTED;
                    set_color_red();
                    sound_reject();
                    continue;
                }
            }

            // If the joystick button is pressed, mark the notification as seen
            // (if it is of type RecvFromFriend or ReqAccepted)
            if (is_joy_button_pressed()) {
                // Check the type of the notif.
                Notification &notif = users[logged_user].notifications[curr_notif];
                if (notif.type == NotifType::RecvFromFriend || notif.type == NotifType::ReqAccepted) {

                    uint8_t left = mark_notif_as_seen(logged_user, curr_notif);

                    // If that was the only notification, go to NOTIFICATIONS_NO_NEW menu.
                    if (left == 0) {
                        curr_menu = Menu::NOTIFICATIONS_NO_NEW;
                        return;
                    }

                    // If curr_notif was the last, decrement it by 1, else leave it the same.
                    // It can't be the last AND be 0, because that would mean left == 0,
                    // which is treated above.
                    if (curr_notif == left) {
                        curr_notif--;
                    }

                    display_notification(users[logged_user].notifications[curr_notif]);
                    continue;
                }
            }

            // If red button is pressed, go to LOGGED_NOTIFS menu.
            if (is_red_button_pressed()) {
                curr_menu = Menu::LOGGED_NOTIFS;
                return;
            }
        }

        else if (mode == NotifMode::ACCEPTED || mode == NotifMode::REJECTED) {
            // Menu after accepting/rejecting a friend request.

            // If joystick button is pressed, go back to displaying notifications.
            if (is_joy_button_pressed()) {
                // Remove the current notification.
                uint8_t left = mark_notif_as_seen(logged_user, curr_notif);

                // If that was the only notification, go to NOTIFICATIONS_NO_NEW menu.
                if (left == 0) {
                    curr_menu = Menu::NOTIFICATIONS_NO_NEW;
                    return;
                }

                if (curr_notif == left) {
                    curr_notif--;
                }

                display_notification(users[logged_user].notifications[curr_notif]);

                mode = NotifMode::VIEW;
                set_color_purple();
                continue;
            }
        }

        else {
            // Should never be reached.
            curr_menu = Menu::ERROR;
            return;
        }
    }
}


void MENU_NOTIFICATIONS_no_new() {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(F("No new notif"));
    set_color_purple();

    while (true) {
        // If red button is pressed, go to LOGGED_NOTIFS menu.
        if (is_red_button_pressed()) {
            curr_menu = Menu::LOGGED_NOTIFS;
            return;
        }
    }
}


/*=====================================================================================*/
/* CHANGE_PIN menu */
void MENU_CHANGE_PIN_enter() {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(F("Enter new PIN:"));
    set_color_blue();

    // For safety reasons, read to a uint32_t
    uint32_t pin_big = read_number_input(ReadInputType::PIN);
    uint16_t pin = pin_big % 10000;

    if (pin == 0) {
        // Red button was pressed, abort PIN change.
        curr_menu = Menu::LOGGED_CHANGE_PIN;
        return;
    }

    users[logged_user].pin = pin;
    curr_menu = Menu::CHANGE_PIN_DONE;
}


void MENU_CHANGE_PIN_done() {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(F("New PIN saved"));
    set_color_blue();
    sound_accept();

    while (true) {
        if (is_red_button_pressed()) {
            curr_menu = Menu::LOGGED_HELLO;
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
    set_color_yellow();

    uint32_t sum = read_number_input(ReadInputType::SUM);

    if (sum == 0) {
        // Red button was pressed, go back to previous menu.
        if (enter_sum_type == EnterSum::ADD_CASH) curr_menu = Menu::MAIN_ACC_ADD;
        else if (enter_sum_type == EnterSum::MAIN_TO_ECO) curr_menu = Menu::MAIN_ACC_TO_ECO;
        else if (enter_sum_type == EnterSum::ECO_TO_MAIN) curr_menu = Menu::ECO_ACC_TO_MAIN;
        else if (enter_sum_type == EnterSum::PAY) curr_menu = Menu::MAIN_ACC_PAY;
        else if (enter_sum_type == EnterSum::SEND_FRIEND) curr_menu = Menu::SEND_FRIEND_CHOOSE;

        enter_sum_type = EnterSum::NO_ENTER;
        return;
    }

    if (enter_sum_type == EnterSum::ADD_CASH) {
        users[logged_user].checking_sum += sum;
        curr_menu = Menu::TRANSACTION_DONE;
    }
    else if (enter_sum_type == EnterSum::MAIN_TO_ECO) {
        if (users[logged_user].checking_sum >= sum) {
            apply_interest(users[logged_user]);

            users[logged_user].checking_sum -= sum;
            users[logged_user].economy_sum += sum;
            curr_menu = Menu::TRANSACTION_DONE;
        } else {
            curr_menu = Menu::NO_FUNDS;
        }
    }
    else if (enter_sum_type == EnterSum::ECO_TO_MAIN) {
        apply_interest(users[logged_user]);

        if (users[logged_user].economy_sum >= (float)sum) {
            users[logged_user].economy_sum -= sum;
            users[logged_user].checking_sum += sum;
            curr_menu = Menu::TRANSACTION_DONE;
        } else {
            curr_menu = Menu::NO_FUNDS;
        }
    }
    else if (enter_sum_type ==  EnterSum::PAY) {
        if (users[logged_user].checking_sum >= sum) {
            users[logged_user].checking_sum -= sum;
            curr_menu = Menu::TRANSACTION_DONE;
        } else {
            curr_menu = Menu::NO_FUNDS;
        }
    }
    else if (enter_sum_type == EnterSum::SEND_FRIEND) {
        if (friend_to_send_money == NO_USER) {
            enter_sum_type = EnterSum::NO_ENTER;
            curr_menu = Menu::ERROR;
            return;
        }

        if (users[logged_user].checking_sum >= sum) {
            users[logged_user].checking_sum -= sum;
            users[friend_to_send_money].checking_sum += sum;

            // Add a notification to the receiver inbox.
            add_notification_to_inbox(friend_to_send_money, logged_user,
                                      NotifType::RecvFromFriend, sum);

            curr_menu = Menu::TRANSACTION_DONE;

            // Reset friend_to_send_money
            friend_to_send_money = NO_USER;

        } else {
            curr_menu = Menu::NO_FUNDS;
        }
    }
    else {
        // Should never be reached.
        enter_sum_type = EnterSum::NO_ENTER;
        curr_menu = Menu::ERROR;
    }
}


/*=====================================================================================*/
/* TRANSACTION_DONE menu */
void MENU_TRANSACTION_DONE_done() {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(F("Transaction"));
    lcd.setCursor(0, 1);
    lcd.print(F("successful"));

    set_color_green();
    sound_transaction_successful();

    while (true) {
        // If red button is pressed, go back to previous menu.
        if (is_red_button_pressed()) {
            if (enter_sum_type == EnterSum::ADD_CASH) curr_menu = Menu::MAIN_ACC_SUM;
            else if (enter_sum_type == EnterSum::MAIN_TO_ECO) curr_menu = Menu::MAIN_ACC_SUM;
            else if (enter_sum_type == EnterSum::ECO_TO_MAIN) curr_menu = Menu::ECO_ACC_SUM;
            else if (enter_sum_type == EnterSum::PAY) curr_menu = Menu::MAIN_ACC_SUM;
            else if (enter_sum_type == EnterSum::SEND_FRIEND) curr_menu = Menu::MAIN_ACC_SUM;

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
    lcd.print(F("Not enough funds"));

    set_color_red();
    sound_transaction_failed();

    while (true) {
        // If red button is pressed, go back to previous menu.
        if (is_red_button_pressed()) {
            if (enter_sum_type == EnterSum::ADD_CASH) curr_menu = Menu::MAIN_ACC_SUM;
            else if (enter_sum_type == EnterSum::MAIN_TO_ECO) curr_menu = Menu::MAIN_ACC_SUM;
            else if (enter_sum_type == EnterSum::ECO_TO_MAIN) curr_menu = Menu::ECO_ACC_SUM;
            else if (enter_sum_type == EnterSum::PAY) curr_menu = Menu::MAIN_ACC_SUM;
            else if (enter_sum_type == EnterSum::SEND_FRIEND) curr_menu = Menu::MAIN_ACC_SUM;

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
    lcd.print(get_wdt_counter());
    set_color_yellow();

    while (true) {
        // If the joystick is pushed up, reprint the counter.
        if (joystick_to_up()) {
            lcd.setCursor(0, 1);
            lcd.print(BLANK);
            lcd.setCursor(0, 1);
            lcd.print(get_wdt_counter());
        }

        // If the red button is pressed, return to START menu.
        if (is_red_button_pressed()) {
            curr_menu = Menu::START_DEBUG;
            return;
        }
    }
}
