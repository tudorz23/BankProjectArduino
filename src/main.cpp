#include "utils.h"
#include "menus.h"
#include <avr/wdt.h>

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
Menu curr_menu = Menu::NO_MENU;

// Indicates to the ENTER_MENU what action to perform.
EnterSum enter_sum_type = EnterSum::NO_ENTER;

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

// To be incremented by the WDT ISR.
volatile uint16_t wdt_counter = 0;

// frienships[i][j] = true <=> users i and j are friends
bool friendships[MAX_USERS][MAX_USERS] = { false };


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


// WDT Interrupt Service Routine
ISR(WDT_vect) {
    wdt_counter++;
}


// Setup the Watchdog timer to generate an interrupt every 1 second,
// to manage an interest gain in the economy accounts.
void watchdog_setup() {
    // Temporarily disable interrupts
    cli();

    // CLear Watchdog System Reset Flag
    MCUSR &= ~(1 << WDRF);

    // Set Watchdog Change Enable and Watchdog System Reset Enable
    WDTCSR |= (1 << WDCE) | (1 << WDE);

    // Set interrupt mode and set timeout value to 1s
    WDTCSR = (1 << WDIE) | (1 << WDP2) | (1 << WDP1);

    // Re-enable interrupts.
    sei();
}


void setup() {
    // USART communication at 9600 baud.
    Serial.begin(9600);

    // Init LCD.
    lcd.init();
    lcd.clear();
    lcd.backlight();

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

    analogWrite(LED_R_PIN, 0);
    analogWrite(LED_G_PIN, 0);
    analogWrite(LED_B_PIN, 0);

    // Init buzzer and set to off.
    pinMode(BUZZER_PIN, OUTPUT);
    noTone(BUZZER_PIN);

    // Initialize names and UIDs.
    init_database();

    // Start from the HELLO menu.
    curr_menu = Menu::START_HELLO;

    // Setup the WDT.
    watchdog_setup();
}


void loop() {
    switch(curr_menu) {
    // START menus
    case Menu::START_HELLO:
        MENU_START_hello();
        break;
    case Menu::START_LOGIN:
        MENU_START_login();
        break;
    case Menu::START_REGISTER:
        MENU_START_register();
        break;
    case Menu::START_DEBUG:
        MENU_START_debug();
        break;

    // REGISTER menus
    case Menu::REGISTER_SCAN:
        MENU_REGISTER_scan();
        break;
    case Menu::REGISTER_ALREADY_REG:
        MENU_REGISTER_already_reg();
        break;
    case Menu::REGISTER_PIN:
        MENU_REGISTER_pin();
        break;

    // LOGIN menus
    case Menu::LOGIN_SCAN:
        MENU_LOGIN_scan();
        break;
    case Menu::LOGIN_NOT_REGISTERED:
        MENU_LOGIN_not_registered();
        break;
    case Menu::LOGIN_ENTER_PIN:
        MENU_LOGIN_enter_pin();
        break;
    case Menu::LOGIN_WRONG_PIN:
        MENU_LOGIN_wrong_pin();
        break;

    // LOGGED menus
    case Menu::LOGGED_HELLO:
        MENU_LOGGED_hello();
        break;
    case Menu::LOGGED_MAIN_ACC:
        MENU_LOGGED_main_acc();
        break;
    case Menu::LOGGED_ECO_ACC:
        MENU_LOGGED_eco_acc();
        break;
    case Menu::LOGGED_LOGOUT:
        MENU_LOGGED_logout();
        break;
    case Menu::LOGGED_FRIENDS:
        MENU_LOGGED_friends();
        break;

    // MAIN_ACC menus
    case Menu::MAIN_ACC_SUM:
        MENU_MAIN_ACC_sum();
        break;
    case Menu::MAIN_ACC_ADD:
        MENU_MAIN_ACC_add();
        break;
    case Menu::MAIN_ACC_PAY:
        MENU_MAIN_ACC_pay();
        break;
    case Menu::MAIN_ACC_TO_ECO:
        MENU_MAIN_ACC_to_eco();
        break;
    
    // ECO_ACC menus
    case Menu::ECO_ACC_SUM:
        MENU_ECO_ACC_sum();
        break;
    case Menu::ECO_ACC_TO_MAIN:
        MENU_ECO_ACC_to_main();
        break;

    // FRIENDS menus
    case Menu::FRIENDS_SEE:
        MENU_FRIENDS_see();
        break;
    case Menu::FRIENDS_ADD:
        MENU_FRIENDS_add();
        break;

    // VIEW_FRIENDS menus
    case Menu::VIEW_FRIENDS:
        MENU_VIEW_FRIENDS_see();
        break;
    case Menu::VIEW_FRIENDS_NO_FRIEND:
        MENU_VIEW_FRIENDS_no_friend();
        break;

    // ADD_FRIENDS menus.
    case Menu::ADD_FRIENDS:
        MENU_ADD_FRIENDS_add();
        break;
    case Menu::ADD_FRIENDS_NO_CANDIDATE:
        MENU_ADD_FRIENDS_no_candidate();
        break;

    // ENTER_SUM menu
    case Menu::ENTER_SUM:
        MENU_ENTER_sum();
        break;

    // TRANSACTION_DONE menu
    case Menu::TRANSACTION_DONE:
        MENU_TRANSACTION_DONE_done();
        break;

    // NO_FUNDS menu
    case Menu::NO_FUNDS:
        MENU_NO_funds();
        break;

    // DEBUG menus
    case Menu::DEBUG_WDT:
        MENU_DEBUG_wdt();
        break;

    // ERROR menus
    case Menu::ERROR:
        MENU_error();
        break;
    default:
        curr_menu = Menu::ERROR;
        break;
    }
}
