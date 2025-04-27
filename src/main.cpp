#include <Arduino.h>
#include <SPI.h>
#include <MFRC522.h>
#include <LiquidCrystal_I2C.h>
#include <TTP229.h>

#include "utils.h"
#include "menus.h"

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
int8_t logged_user = -1;


// For joystick delay between levels.
unsigned long last_joy_delay_time = 0;

// For buttons debouncing.
unsigned long last_joy_debounce_time = 0;
unsigned long last_red_debounce_time = 0;

int last_joy_button_state = HIGH;
int last_red_button_state = HIGH;



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

    default:
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print(F("WTF..."));
        while (true) {
            delay(500);
        }
        break;
    }
}


/*=====================================================================================*/
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

        // If Joystick button is pressed, access the REGISTER menu.
        if (is_joy_button_pressed()) {
            curr_menu = MENU_REGISTER_SCAN;
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
        if (is_red_button_pressed()) {
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

        // Got a new card, read the UID
        char uid[UID_SIZE + 1];
        extract_uid(uid);
        Serial.println(uid);
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


// Debouncing for the joystick button.
bool is_joy_button_pressed() {
    int curr_state = digitalRead(JOYSTICK_SW_PIN);
    if (curr_state != last_joy_button_state) {
        last_joy_debounce_time = millis();
    }

    last_joy_button_state = curr_state;

    if (millis() - last_joy_debounce_time > DEBOUNCE_DELAY) {
        if (curr_state == LOW) {
            return true;
        }
    }

    return false;
}


// Debouncing for the red button.
bool is_red_button_pressed() {
    int curr_state = digitalRead(RED_BUTTON_PIN);
    if (curr_state != last_red_button_state) {
        last_red_debounce_time = millis();
    }

    last_red_button_state = curr_state;

    if (millis() - last_red_debounce_time > DEBOUNCE_DELAY) {
        if (curr_state == LOW) {
            return true;
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

