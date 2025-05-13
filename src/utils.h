#ifndef _UTILS_H_
#define _UTILS_H_

#include <LiquidCrystal_I2C.h>
#include <MFRC522.h>
#include <TTP229.h>
#include "pins.h"

// #define DEBUG

/*=====================================================================================*/
/* DATA STRUCTURES */

enum class NotifType {
    RecvFromFriend, FriendReq, ReqAccepted
};

struct Notification {
    NotifType type;

    uint8_t from_who;

    uint32_t sum;
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


enum class ReadInputType {
    PIN, SUM
};


/*=====================================================================================*/
/* CONSTANTS */

constexpr int BETWEEN_MENUS_DELAY = 400;
constexpr int JOY_RIGHT_THRESHOLD = 800;
constexpr int JOY_LEFT_THRESHOLD = 200;
constexpr int JOY_UP_THRESHOLD = 200;
constexpr int JOY_DOWN_THRESHOLD = 800;

constexpr int DEBOUNCE_DELAY = 30;

constexpr int8_t NO_USER = -1;
constexpr uint8_t MAX_USERS = 6;
constexpr uint8_t UID_SIZE = 8;
constexpr uint8_t PIN_SIZE = 4;
constexpr uint8_t MAX_SUM_DIGITS = 7;

constexpr uint8_t INTEREST_RATE = 2;
constexpr uint8_t APPLY_INTEREST_INTERVAL = 15;

constexpr uint32_t INITIAL_CHECKING_SUM = 200;
constexpr float INITIAL_ECO_SUM = 100.0;

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

extern bool sent_friend_req[MAX_USERS][MAX_USERS];

extern int8_t friend_to_send_money;


/*=====================================================================================*/
/* HELPER FUNCTIONS */

// Check if the joystick is moved to the right.
bool joystick_to_the_right();

// Check if the joystick is moved to the left.
bool joystick_to_the_left();

// Check if the joystick is moved upwards.
bool joystick_to_up();

// Check if the joystick is moved downwards.
bool joystick_to_down();

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
void register_user(uint8_t idx);

// Returns true if the user is registered, false otherwise.
bool is_user_registered(uint8_t idx);

// Updates the economy sum of the user, applying interest based
// on the WDT indications.
void apply_interest(User &user);

// Searches for the lowest user index that is a friend of the logged_user.
// If there is no such index, returns -1.
int8_t get_first_friend(uint8_t logged_user);

// Searches for the previous friend of the logged_user.
uint8_t get_prev_friend(uint8_t logged_user, uint8_t curr_friend);

// Searches for the next friend of the logged_user.
uint8_t get_next_friend(uint8_t logged_user, uint8_t curr_friend);

// Searches for the lowest registered user index that is not a friend of the logged_user.
// If there is no such index, returns -1.
int8_t get_first_friend_candidate(uint8_t logged_user);

// Searches for the previous friend candidate of the logged_user.
uint8_t get_prev_friend_candidate(uint8_t logged_user, uint8_t curr_candidate);

// Searches for the next friend candidate of the logged_user.
uint8_t get_next_friend_candidate(uint8_t logged_user, uint8_t curr_candidate);

// DIsplays the notification on the LCD, depending on its type.
void display_notification(Notification &notif);

// Searches for the previous notification.
uint8_t get_prev_notification(uint8_t curr_notif);

// Searches for the next notification, where notif_cnt is the total number of
// notifications that the user currenlty has.
uint8_t get_next_notification(uint8_t notif_cnt, uint8_t curr_notif);

// Adds the specified notif to to_who's inbox.
void add_notification_to_inbox(uint8_t to_who, uint8_t from_who, NotifType type, uint32_t sum);

// Removes the specified notif from logged_user's inbox.
// Slides the following notifications one place to the left.
// Returns logged_user's number of notifications left.
uint8_t mark_notif_as_seen(uint8_t logged_user, uint8_t notif_idx);

#endif
