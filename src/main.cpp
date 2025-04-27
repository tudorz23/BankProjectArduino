#include <Arduino.h>
#include <SPI.h>
#include <MFRC522.h>
#include <LiquidCrystal_I2C.h>
#include <TTP229.h>

#include "utils.h"
#include "menus.h"

#define DEBUG

/* DECLARE COMPONENT OBJECTS */ 
// LCD (address 0x27, 16 chars and 2 lines)
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Card reader
MFRC522 mfrc522(RFID_SDA_PIN, RFID_RST_PIN);

// TTP229 keyboard
TTP229 ttp229(TTP_SCL_PIN, TTP_SDO_PIN);


/* DECLARE GLOBAL VARIABLES */
// Pre-defined users.
User users[MAX_USERS];

// users[idx] has names[idx] name.
const char *names[MAX_USERS];

// users[idx] has uids[idx] uid.
const char *uids[MAX_USERS];

// Used with bit manipulation (i.e. user i is active -> bit i is 1, else 0).
uint8_t registered_users = 0;

// The menu currently running.
uint8_t curr_menu = NO_MENU;

// The index of the currently logged-in user (initially nobody).
int8_t logged_user = NO_USER;


// For joystick delay between levels.
unsigned long last_joy_delay_time = 0;

// For buttons debouncing.
unsigned long last_joy_debounce_time = 0;
unsigned long last_red_debounce_time = 0;

int last_joy_button_state = HIGH;
int joy_button_stable_state = HIGH;
int last_red_button_state = HIGH;
int red_button_stable_state = HIGH;



void init_database() {
    names[0] = "Lewis Hamilton";
    names[1] = "Charles Leclerc";
    names[2] = "Oscar Piastri";
    names[3] = "Leo Messi";
    names[4] = "Roger Federer";
    names[5] = "Lamine Yamal";

    uids[0] = "3733EF00";
    uids[1] = "3DD84F00";
    uids[2] = "9EEA4200";
    uids[3] = "EECA3200";
    uids[4] = "43293000";
    uids[5] = "C6F04800";
}


void setup() {
    // USART communication at 9600 baud.
    Serial.begin(9600);

    // Init LCD.
    lcd.init();
	lcd.clear();
    lcd.backlight();
    lcd.print(F("Calibrating..."));

    // Init card reader.
    SPI.begin();
    mfrc522.PCD_Init();

    // Init buttons.
    pinMode(JOYSTICK_SW_PIN, INPUT_PULLUP);
    pinMode(RED_BUTTON_PIN, INPUT);

    // Init RGB led and set to off.
    pinMode(LED_R_PIN, OUTPUT);
    pinMode(LED_G_PIN, OUTPUT);
    pinMode(LED_B_PIN, OUTPUT);

    analogWrite(LED_R_PIN, 150);
    analogWrite(LED_G_PIN, 0);
    analogWrite(LED_B_PIN, 0);

    // Init buzzer and set to off.
    pinMode(BUZZER_PIN, OUTPUT);
    noTone(BUZZER_PIN);

    // Initialize names and UIDs.
    init_database();

    // Start from the HELLO menu.
    curr_menu = MENU_MAIN_HELLO;

    delay(2000);
}


void loop() {
    switch(curr_menu) {
    case MENU_MAIN_HELLO:
        MENU_MAIN_hello();
        break;
    case MENU_MAIN_LOGIN:
        MENU_MAIN_login();
        break;
    case MENU_MAIN_REGISTER:
        MENU_MAIN_register();
        break;
    case MENU_REGISTER_SCAN:
        MENU_REGISTER_scan();
        break;
    case MENU_REGISTER_PIN:
        MENU_REGISTER_pin();
        break;
    case MENU_LOGGED_HELLO:
        MENU_LOGGED_hello();
        break;


    case MENU_ERROR:
        MENU_error();
        break;
    default:
        curr_menu = MENU_ERROR;
        break;
    }
}


/*=====================================================================================*/
/* MENUS IMPLEMENTATIONS */
void MENU_error() {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(F("ERROR"));

    while (true) {
        if (is_button_pressed(RED_BUTTON_PIN, last_red_button_state,
                              red_button_stable_state, last_red_debounce_time)) {
            curr_menu = MENU_MAIN_HELLO;
            return;
        }
    }
}


void MENU_MAIN_hello() {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(F("Welcome!"));

    while (true) {
        if (joystick_to_the_right()) {
            curr_menu = MENU_MAIN_LOGIN;
            return;
        }
    }
}


void MENU_MAIN_login() {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(F("Login"));

    while (true) {
        if (joystick_to_the_left()) {
            curr_menu = MENU_MAIN_HELLO;
            return;
        }

        if (joystick_to_the_right()) {
            curr_menu = MENU_MAIN_REGISTER;
            return;
        }
    }
}


void MENU_MAIN_register() {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(F("Register"));

    while (true) {
        if (joystick_to_the_left()) {
            curr_menu = MENU_MAIN_LOGIN;
            return;
        }

        // If Joystick button is pressed, access the REGISTER menu.
        if (is_button_pressed(JOYSTICK_SW_PIN, last_joy_button_state,
                              joy_button_stable_state, last_joy_debounce_time)) {
            curr_menu = MENU_REGISTER_SCAN;
            return;
        }

        if (is_button_pressed(RED_BUTTON_PIN, last_red_button_state,
                              red_button_stable_state, last_red_debounce_time)) {
            curr_menu = MENU_MAIN_HELLO;
            return;
        }
    }
}


void MENU_REGISTER_scan() {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(F("Scan card"));

    while (true) {
        // If red button is pressed, abort and go back to MAIN menu.
        if (is_button_pressed(RED_BUTTON_PIN, last_red_button_state,
                              red_button_stable_state, last_red_debounce_time)) {
            curr_menu = MENU_MAIN_REGISTER;
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
    users[logged_user].checking_sum = 0;
    users[logged_user].economy_sum = 0;
    users[logged_user].pin = pin;

    curr_menu = MENU_LOGGED_HELLO;
}


void MENU_LOGGED_hello() {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(F("Hello"));

    lcd.setCursor(0, 1);
    lcd.print(names[logged_user]);

    while (true) {
        if (is_button_pressed(RED_BUTTON_PIN, last_red_button_state,
                              red_button_stable_state, last_red_debounce_time)) {
            logged_user = -1;
            curr_menu = MENU_MAIN_HELLO;
            return;
        }
    }
}


/*=====================================================================================*/
/* FUNCTIONS - MECHANICAL */
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


/*=====================================================================================*/
/* FUNCTIONS - SOFTWARE */
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


uint16_t get_pin_input() {
    uint16_t pin = 0;
    uint8_t read_digit = 0;

    uint8_t idx = 0;
    uint8_t last_key = 0;  // Keep track of last key pressed

    while (true) {
        // If red button is pressed ,return 0 to signal abort.
        if (is_button_pressed(RED_BUTTON_PIN, last_red_button_state,
                              red_button_stable_state, last_red_debounce_time)) {
            return 0;
        }

        // If joystick button is pressed and full pin is introduced, return the pin.
        if (idx == PIN_SIZE && is_button_pressed(JOYSTICK_SW_PIN, last_joy_button_state,
                              joy_button_stable_state, last_joy_debounce_time)) {
            return pin;
        }

        // Read current key
        read_digit = ttp229.GetKey16(); // non-blocking

        if (read_digit != 0 && last_key == 0) {
            // Only react if no key was previously pressed (some sort of debouncing)
            if (read_digit == 10) { // '0' key
                if (idx != 0) {
                    pin = pin * 10;
                    idx++;
                }
            }

            else if (read_digit == 11) { // backspace
                if (idx != 0) {
                    pin = pin / 10;
                    idx--;
                }
            }

            else if (read_digit <= 9) { // normal digits
                if (idx < PIN_SIZE) {
                    pin = pin * 10 + read_digit;
                    idx++;
                }
            }

            // Update LCD
            lcd.setCursor(0, 1);
            lcd.print("    "); // Clear
            lcd.setCursor(0, 1);
            if (pin != 0) {
                lcd.print(pin);
            }
        }

        // Update last_key
        if (read_digit != 0) {
            // key is being pressed
            last_key = read_digit;
        } else {
            // no key is being pressed
            last_key = 0;     
        }

        // // Print intermiedary pin.
        // lcd.setCursor(0, 1);
        // lcd.print("    "); // clear the line
        // lcd.setCursor(0, 1);
        // if (pin != 0) {
        //     lcd.print(pin);
        // }

        // read_digit = ttp229.GetKey16();

        // // Ignore key release and keys larger than 11.
        // if (read_digit == 0 || read_digit > 11) {
        //     continue;
        // }

        // // Don't react if the same key is read over and over again.
        // if (last_key != 0) {
        //     continue;
        // }

        // // 10 is used to input 0.
        // if (read_digit == 10) {
        //     // Don't allow 0 as the first digit.
        //     if (idx == 0) {
        //         continue;
        //     }

        //     read_digit = 0;
        // }

        // // 11 is used as backspace.
        // if (read_digit == 11) {
        //     // Cannot delete if there is no digit.
        //     if (idx == 0) {
        //         continue;
        //     }

        //     pin = pin / 10;
        //     idx--;
        //     continue;
        // }

        // // Successfuly got idx'th digit.
        // if (idx < PIN_SIZE) {
        //     pin = pin * 10 + read_digit;
        // }

        // idx++;
    }
}
