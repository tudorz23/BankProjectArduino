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

#define MENU_LOGIN_SCAN 7
#define MENU_LOGIN_NOT_REGISTERED 8
#define MENU_LOGIN_ENTER_PIN 9
#define MENU_LOGIN_WRONG_PIN 10

#define MENU_LOGGED_HELLO 11
#define MENU_LOGGED_LOGOUT 12

/* MENU FUNCTIONS */
void MENU_error();

void MENU_MAIN_hello();
void MENU_MAIN_login();
void MENU_MAIN_register();

void MENU_REGISTER_scan();
void MENU_REGISTER_pin();

void MENU_LOGIN_scan();
void MENU_LOGIN_not_registered();
void MENU_LOGIN_enter_pin();
void MENU_LOGIN_wrong_pin();

void MENU_LOGGED_hello();
void MENU_LOGGED_logout();



/* OTHER CONSTANT STUFF */
#define BETWEEN_MENUS_DELAY 400
#define JOY_RIGHT_THRESHOLD 800
#define JOY_LEFT_THRESHOLD 200

#endif
