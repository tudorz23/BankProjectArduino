#ifndef MENUS_H
#define MENUS_H

#include "Arduino.h"


enum class EnterSum {
    NO_ENTER, ADD_CASH, MAIN_TO_ECO, ECO_TO_MAIN, PAY
};


/* MENU CODES */
enum class Menu {
    NO_MENU, ERROR,

    // START menus
    START_HELLO, START_LOGIN, START_REGISTER, START_DEBUG,

    // REGISTER menus
    REGISTER_SCAN, REGISTER_ALREADY_REG, REGISTER_PIN,

    // LOGIN menus
    LOGIN_SCAN, LOGIN_NOT_REGISTERED, LOGIN_ENTER_PIN, LOGIN_WRONG_PIN,

    // LOGGED menus
    LOGGED_HELLO, LOGGED_MAIN_ACC, LOGGED_ECO_ACC, LOGGED_LOGOUT,

    // MAIN_ACC menus
    MAIN_ACC_SUM, MAIN_ACC_ADD, MAIN_ACC_PAY, MAIN_ACC_TO_ECO,

    // ECO_ACC menus
    ECO_ACC_SUM, ECO_ACC_TO_MAIN,

    // ENTER_SUM menu
    ENTER_SUM,

    // DONE menu
    DONE,

    // NO_FUNDS menu
    NO_FUNDS,

    // DEBUG menus
    DEBUG_WDT
};


/* GLOBAL VARIABLES */
extern Menu curr_menu;
extern EnterSum enter_sum_type;


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
void MENU_REGISTER_scan(); // Alters `logged_user`
void MENU_REGISTER_already_reg();
void MENU_REGISTER_pin(); // Alters `registered_user`, `users[logged_user]`


/*=====================================================================================*/
/* LOGIN menus */
void MENU_LOGIN_scan(); // Alters `logged_user`
void MENU_LOGIN_not_registered();
void MENU_LOGIN_enter_pin(); // Alters `logged_user`
void MENU_LOGIN_wrong_pin();


/*=====================================================================================*/
/* LOGGED menus */
void MENU_LOGGED_hello();
void MENU_LOGGED_main_acc();
void MENU_LOGGED_eco_acc();
void MENU_LOGGED_logout(); // Alters `logged_user`


/*=====================================================================================*/
/* MAIN_ACC menus */
void MENU_MAIN_ACC_sum();
void MENU_MAIN_ACC_add();
void MENU_MAIN_ACC_pay();
void MENU_MAIN_ACC_to_eco();


/*=====================================================================================*/
/* ECO_ACC menus */
void MENU_ECO_ACC_sum();
void MENU_ECO_ACC_to_main();


/*=====================================================================================*/
/* ENTER_SUM menu */
void MENU_ENTER_sum();


/*=====================================================================================*/
/* DONE menu */
void MENU_DONE_done(); // can only be reached via Menu::ENTER_SUM


/*=====================================================================================*/
/* NO_FUNDS menu */
void MENU_NO_funds(); // can only be reached via Menu::ENTER_SUM


/*=====================================================================================*/
/* DEBUG menus */
void MENU_DEBUG_wdt();

#endif
