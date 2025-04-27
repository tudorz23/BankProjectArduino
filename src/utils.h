#ifndef UTILS_H
#define UTILS_H

#include <Arduino.h>

/* USER STRUCTURE */
struct User {
    // Sum in the checking account
    uint32_t checking_sum;

    // Sum in the economy account
    float economy_sum;

    // PIN code
    uint16_t pin;
};


/* COMPONENT PINS */
// Card reader
const int RFID_SDA_PIN = 8;
const int RFID_RST_PIN = 7;

// Joystick
const int JOYSTICK_VRX_PIN = A0;
const int JOYSTICK_VRY_PIN = A1;
const int JOYSTICK_SW_PIN = A2;

// TTP229 keyboard
const int TTP_SCL_PIN = 4; // The pin number of the clock pin.
const int TTP_SDO_PIN = 2; // The pin number of the data pin.

// RGB led
const int LED_R_PIN = 10;
const int LED_G_PIN = 9;
const int LED_B_PIN = 6;

// Buzzer
const int BUZZER_PIN = 3;

// Red button
const int RED_BUTTON_PIN = 5;


/* OTHER CONSTANTS */
const int DEBOUNCE_DELAY = 30;

const int8_t NO_USER = -1;
const uint8_t MAX_USERS = 6;
const uint8_t UID_SIZE = 8;
const uint8_t PIN_SIZE = 4;


/* FUNCTIONS - MECHANICAL */
// Check if the joystick is moved to the right.
bool joystick_to_the_right();

// Check if the joystick is moved to the left.
bool joystick_to_the_left();

// Check if a button is pressed (with debouncing).
bool is_button_pressed(const int pin, int &last_state, int &stable_state,
                       unsigned long &last_debounce_time);


/* FUNCTIONS - SOFTWARE */
// Extract the uid from the mfrc522 and save it as a char array,
// in HEX representation.
void extract_uid(char *buff);


int8_t get_user_idx_from_uid(char *uid);

// Reads a 4-digit PIN code from the TTP229.
// Return the PIN if successful, or 0 if red button was pressed.
uint16_t get_pin_input();


// Marks an user as registered using bitwise operations.
void register_user(int8_t idx);

// Returns true if the user is registered, false otherwise.
bool is_user_registered(int8_t idx);

#endif
