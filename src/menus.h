#ifndef MENUS_H
#define MENUS_H

#include "Arduino.h"


enum EnterSumType {
    NO_ENTER, ADD_CASH, MAIN_TO_ECO, ECO_TO_MAIN, PAY
};


/* GLOBAL VARIABLE */
extern uint8_t curr_menu;
extern EnterSumType enter_sum_type;


/* MENU CODES */
constexpr uint8_t NO_MENU = 0;
constexpr uint8_t MENU_ERROR = 1;

// START menus
constexpr uint8_t MENU_START_HELLO = 10;
constexpr uint8_t MENU_START_LOGIN = 11;
constexpr uint8_t MENU_START_REGISTER = 12;
constexpr uint8_t MENU_START_DEBUG = 13;

// REGISTER menus
constexpr uint8_t MENU_REGISTER_SCAN = 20;
constexpr uint8_t MENU_REGISTER_ALREADY_REG = 21;
constexpr uint8_t MENU_REGISTER_PIN = 22;

// LOGIN menus
constexpr uint8_t MENU_LOGIN_SCAN = 30;
constexpr uint8_t MENU_LOGIN_NOT_REGISTERED = 31;
constexpr uint8_t MENU_LOGIN_ENTER_PIN = 32;
constexpr uint8_t MENU_LOGIN_WRONG_PIN = 33;

// LOGGED menus
constexpr uint8_t MENU_LOGGED_HELLO = 40;
constexpr uint8_t MENU_LOGGED_MAIN_ACC = 41;
constexpr uint8_t MENU_LOGGED_ECO_ACC = 42;
constexpr uint8_t MENU_LOGGED_LOGOUT = 49;

// MAIN_ACC menus
constexpr uint8_t MENU_MAIN_ACC_SUM = 50;
constexpr uint8_t MENU_MAIN_ACC_ADD = 51;
constexpr uint8_t MENU_MAIN_ACC_PAY = 52;
constexpr uint8_t MENU_MAIN_ACC_TO_ECO = 53;

// ECO_ACC menus
constexpr uint8_t MENU_ECO_ACC_SUM = 60;
constexpr uint8_t MENU_ECO_ACC_TO_MAIN = 61;

// ENTER_SUM menu
constexpr uint8_t MENU_ENTER_SUM = 100;

// DONE menu
constexpr uint8_t MENU_DONE = 110;

// NO_FUNDS menu
constexpr uint8_t MENU_NO_FUNDS = 120;

// DEBUG menus
constexpr uint8_t MENU_DEBUG_WDT = 130;


/*=====================================================================================*/
/* ERROR menu */
void MENU_error();


/*=====================================================================================*/
/* START menus */
void MENU_START_hello();

void MENU_START_login();

void MENU_START_register();

void MENU_START_debug();


/*=====================================================================================*/
/* REGISTER menus */

// Alters `logged_user`
void MENU_REGISTER_scan();

void MENU_REGISTER_already_reg();

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
void MENU_LOGGED_main_acc();
void MENU_LOGGED_eco_acc();
void MENU_LOGGED_logout();


/*=====================================================================================*/
/* MAIN_ACC menus */
void MENU_MAIN_ACC_sum();
void MENU_MAIN_ACC_add();
void MENU_MAIN_ACC_pay();
void MENU_MAIN_ACC_to_eco();


/*=====================================================================================*/
/* ECO_ACC menus */
void MENU_ECO_ACC_sum();


/*=====================================================================================*/
/* ENTER_SUM menu */
void MENU_ENTER_sum();


/*=====================================================================================*/
/* DONE menu */
void MENU_DONE_done(); // can only be reached via MENU_ENTER_SUM


/*=====================================================================================*/
/* NO_FUNDS menu */
void MENU_NO_funds(); // can only be reached via MENU_ENTER_SUM

/*=====================================================================================*/
/* DEBUG menus */
void MENU_DEBUG_wdt();

#endif
