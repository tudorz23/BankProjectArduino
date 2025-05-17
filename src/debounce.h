#ifndef _DEBOUNCE_H_
#define _DEBOUNCE_H_

/* CONSTANTS */
constexpr int BETWEEN_MENUS_DELAY = 400;
constexpr int JOY_RIGHT_THRESHOLD = 800;
constexpr int JOY_LEFT_THRESHOLD = 200;
constexpr int JOY_UP_THRESHOLD = 200;
constexpr int JOY_DOWN_THRESHOLD = 800;
constexpr int DEBOUNCE_DELAY = 30;


/*=====================================================================================*/
/* FUNCTION DECLARATIONS*/

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

// Check if joystick button is pressed (with debouncing).
bool is_joy_button_pressed();

// Check if red button is pressed (with debouncing).
bool is_red_button_pressed();

#endif
