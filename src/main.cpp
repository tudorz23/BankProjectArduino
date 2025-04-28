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
    curr_menu = MENU_START_HELLO;

    delay(2000);
}


void loop() {
    switch(curr_menu) {
    case MENU_START_HELLO:
        MENU_START_hello();
        break;
    case MENU_START_LOGIN:
        MENU_START_login();
        break;
    case MENU_START_REGISTER:
        MENU_START_register();
        break;

    case MENU_REGISTER_SCAN:
        MENU_REGISTER_scan();
        break;
    case MENU_REGISTER_PIN:
        MENU_REGISTER_pin();
        break;

    case MENU_LOGIN_SCAN:
        MENU_LOGIN_scan();
        break;
    case MENU_LOGIN_NOT_REGISTERED:
        MENU_LOGIN_not_registered();
        break;
    case MENU_LOGIN_ENTER_PIN:
        MENU_LOGIN_enter_pin();
        break;
    case MENU_LOGIN_WRONG_PIN:
        MENU_LOGIN_wrong_pin();
        break;

    case MENU_LOGGED_HELLO:
        MENU_LOGGED_hello();
        break;
    case MENU_LOGGED_LOGOUT:
        MENU_LOGGED_logout();
        break;


    case MENU_ERROR:
        MENU_error();
        break;
    default:
        curr_menu = MENU_ERROR;
        break;
    }
}
