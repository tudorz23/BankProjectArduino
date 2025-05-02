#ifndef UTILS_H
#define UTILS_H

#include <Arduino.h>
#include <LiquidCrystal_I2C.h>
#include <MFRC522.h>
#include <TTP229.h>

// #define DEBUG

/* COMPONENT PINS */
// Card reader
constexpr int RFID_SDA_PIN = 8;
constexpr int RFID_RST_PIN = 7;

// Joystick
constexpr int JOYSTICK_VRX_PIN = A0;
constexpr int JOYSTICK_VRY_PIN = A1;
constexpr int JOYSTICK_SW_PIN = A2;

// TTP229 keyboard
constexpr int TTP_SCL_PIN = 4; // The pin number of the clock pin.
constexpr int TTP_SDO_PIN = 2; // The pin number of the data pin.

// RGB led
constexpr int LED_R_PIN = 10;
constexpr int LED_G_PIN = 9;
constexpr int LED_B_PIN = 6;

// Buzzer
constexpr int BUZZER_PIN = 3;

// Red button
constexpr int RED_BUTTON_PIN = 5;


/*=====================================================================================*/
/* DATA STRUCTURES */

struct Notification {
    uint8_t type;

    uint8_t from_who;

    uint16_t sum;
};

constexpr uint8_t MAX_NOTIFS = 4;

/* USER STRUCTURE */
struct User {
    // Sum in the checking account
    uint32_t checking_sum;

    // Sum in the economy account (with interest)
    float economy_sum;

    // Value of the wdt_counter when the user has last had interest added.
    uint16_t last_interest_update_time;

    // PIN code
    uint16_t pin;

    Notification notifications[MAX_NOTIFS];

    uint8_t notif_cnt;
};


enum ReadInputType {
    PIN, SUM
};


/*=====================================================================================*/
/* CONSTANTS */

constexpr int BETWEEN_MENUS_DELAY = 400;
constexpr int JOY_RIGHT_THRESHOLD = 800;
constexpr int JOY_LEFT_THRESHOLD = 200;
constexpr int JOY_UP_THRESHOLD = 200;

constexpr int DEBOUNCE_DELAY = 30;

constexpr int8_t NO_USER = -1;
constexpr uint8_t MAX_USERS = 6;
constexpr uint8_t UID_SIZE = 8;
constexpr uint8_t PIN_SIZE = 4;
constexpr uint8_t MAX_SUM_DIGITS = 7;

constexpr uint8_t INTEREST_RATE = 2;
constexpr uint8_t APPLY_INTEREST_INTERVAL = 15;

constexpr char BLANK[] = "       ";


/*=====================================================================================*/
/* GLOBAL VARIABLES (shared with other files) */

extern LiquidCrystal_I2C lcd;
extern MFRC522 mfrc522;
extern TTP229 ttp229;

extern User users[MAX_USERS];
extern const char *names[MAX_USERS];
extern const char *uids[MAX_USERS];
extern uint8_t registered_users;
extern int8_t logged_user;

extern unsigned long last_joy_delay_time;
extern unsigned long last_joy_debounce_time;
extern unsigned long last_red_debounce_time;

extern int last_joy_button_state;
extern int joy_button_stable_state;
extern int last_red_button_state;
extern int red_button_stable_state;

extern volatile uint16_t wdt_counter;

extern bool friendships[MAX_USERS][MAX_USERS];


/*=====================================================================================*/
/* HELPER FUNCTIONS */

// Check if the joystick is moved to the right.
bool joystick_to_the_right();

// Check if the joystick is moved to the left.
bool joystick_to_the_left();

// Check if the joystick is moved upwards.
bool joystick_to_up();

// Check if a button is pressed (with debouncing).
bool is_button_pressed(const int pin, int &last_state, int &stable_state,
                       unsigned long &last_debounce_time);


// Extract the uid from the mfrc522 and save it as a char array,
// in HEX representation.
void extract_uid(char *buff);

// Gets the index of a user based on the uid of his card.
int8_t get_user_idx_from_uid(char *uid);

// Reads a number from the TTP229.
// If type is PIN, imposes that the read number is 4 digits long.
// If type is SUM, limits the read number to MAX_SUM_DIGITS.
// Returns 0 if the red button was pressed while reading.
uint32_t read_number_input(ReadInputType type);

// Marks an user as registered using bitwise operations.
void register_user(int8_t idx);

// Returns true if the user is registered, false otherwise.
bool is_user_registered(int8_t idx);

// Updates the economy sum of the user, applying interest based
// on the WDT indications.
void apply_interest(User &user);

#endif
