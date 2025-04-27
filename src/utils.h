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
#define DEBOUNCE_TIME 30


const int MAX_USERS = 6;
const int UID_SIZE = 8;



/* FUNCTIONS */

bool joystick_to_the_right();

bool joystick_to_the_left();

void extract_uid(char *buff);


#endif
