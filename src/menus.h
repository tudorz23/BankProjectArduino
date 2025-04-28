#ifndef MENUS_H
#define MENUS_H

#include "Arduino.h"

/* MENU CODES */
#define NO_MENU 0
#define MENU_ERROR 1

#define MENU_START_HELLO 2
#define MENU_START_LOGIN 3
#define MENU_START_REGISTER 4

#define MENU_REGISTER_SCAN 5
#define MENU_REGISTER_PIN 6

#define MENU_LOGIN_SCAN 7
#define MENU_LOGIN_NOT_REGISTERED 8
#define MENU_LOGIN_ENTER_PIN 9
#define MENU_LOGIN_WRONG_PIN 10

#define MENU_LOGGED_HELLO 11
#define MENU_LOGGED_LOGOUT 12


/* GLOBAL VARIABLE */
extern uint8_t curr_menu;


/* ERROR menu */
void MENU_error();


/*=====================================================================================*/
/* START menus */
void MENU_START_hello();

void MENU_START_login();

void MENU_START_register();


/*=====================================================================================*/
/* REGISTER menus */

// Alters `logged_user`
void MENU_REGISTER_scan();

// Alters `registered_user`, `users[logged_user]`
void MENU_REGISTER_pin();


/*=====================================================================================*/
/* LOGIN menus */

// Alters `logged_user`
void MENU_LOGIN_scan();

void MENU_LOGIN_not_registered();

// Alters `logged_user`
void MENU_LOGIN_enter_pin();

void MENU_LOGIN_wrong_pin();


/*=====================================================================================*/
/* LOGGED menus */
void MENU_LOGGED_hello();
void MENU_LOGGED_logout();


#endif
