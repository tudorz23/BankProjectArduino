#include <Arduino.h>
#include <SPI.h>
#include <MFRC522.h>
#include <LiquidCrystal_I2C.h>
#include <TTP229.h>
#include <ezButton.h>

#include "utils.h"
#include "menus.h"

/* DECLARE COMPONENT OBJECTS */ 
// LCD (address 0x27, 16 chars and 2 lines)
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Card reader
MFRC522 mfrc522(RFID_SDA_PIN, RFID_RST_PIN);

// TTP229 keyboard
TTP229 ttp229(TTP_SCL_PIN, TTP_SDO_PIN);

// Joystick button
ezButton joystick_button(JOYSTICK_SW_PIN);

// Red button
ezButton red_button(RED_BUTTON_PIN, EXTERNAL_PULLUP);


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
int8_t logged_user = -1;


// For joystick delay between levels.
unsigned long last_joy_delay_time = 0;

// Joystick coordinates
uint16_t joystick_x = 0;
uint16_t joystick_y = 0;


void init_database() {
    names[0] = "Lewis Hamilton";
    names[1] = "Charles Leclerc";
    names[2] = "Oscar Piastri";
    names[3] = "Leo Messi";
    names[4] = "Roger Federer";
    names[5] = "Lamine Yamal";

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
    joystick_button.setDebounceTime(DEBOUNCE_TIME);
    red_button.setDebounceTime(DEBOUNCE_TIME);

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

    default:
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("WTF...");
        while (true) {
            delay(500);
        }
        break;
    }
}



/* MENUS IMPLEMENTATIONS */
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
    }
}



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
