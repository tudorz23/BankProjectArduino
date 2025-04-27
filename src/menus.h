#ifndef MENUS_H
#define MENUS_H


/* MENU CODES */
#define NO_MENU 0
#define MENU_ERROR 1
#define MENU_MAIN_HELLO 2
#define MENU_MAIN_LOGIN 3
#define MENU_MAIN_REGISTER 4

#define MENU_REGISTER_SCAN 5
#define MENU_REGISTER_PIN 6

#define MENU_LOGGED_HELLO 7

/* MENU FUNCTIONS */
void MENU_error();

void MENU_MAIN_hello();
void MENU_MAIN_login();
void MENU_MAIN_register();

void MENU_REGISTER_scan();
void MENU_REGISTER_pin();

void MENU_LOGGED_hello();



/* OTHER CONSTANT STUFF */
#define BETWEEN_MENUS_DELAY 400
#define JOY_RIGHT_THRESHOLD 800
#define JOY_LEFT_THRESHOLD 200

#endif
