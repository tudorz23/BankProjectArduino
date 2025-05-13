#ifndef _MENUS_H_
#define _MENUS_H_

enum class EnterSum {
    NO_ENTER, ADD_CASH, MAIN_TO_ECO, ECO_TO_MAIN, PAY, SEND_FRIEND
};

enum class NotifMode {
    VIEW, ACCEPTED, REJECTED
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
    LOGGED_FRIENDS, LOGGED_NOTIFS,

    // MAIN_ACC menus
    MAIN_ACC_SUM, MAIN_ACC_ADD, MAIN_ACC_PAY, MAIN_ACC_TO_ECO, MAIN_ACC_SEND_FRIEND,

    // ECO_ACC menus
    ECO_ACC_SUM, ECO_ACC_TO_MAIN,

    // SEND_FRIEND menus
    SEND_FRIEND_CHOOSE, SEND_FRIEND_NO_FRIEND,

    // FRIENDS menus
    FRIENDS_SEE, FRIENDS_ADD,
    
    // VIEW_FRIENDS menus
    VIEW_FRIENDS, VIEW_FRIENDS_NO_FRIEND,

    // ADD_FRIENDS menus
    ADD_FRIENDS, ADD_FRIENDS_NO_CANDIDATE,

    // NOTIFICATIONS menus
    NOTIFICATIONS_SEE, NOTIFICATIONS_NO_NEW,

    // ENTER_SUM menu
    ENTER_SUM,

    // TRANSACTION_DONE menu
    TRANSACTION_DONE,

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
void MENU_LOGGED_friends();
void MENU_LOGGED_notifications();


/*=====================================================================================*/
/* MAIN_ACC menus */
void MENU_MAIN_ACC_sum();
void MENU_MAIN_ACC_add();
void MENU_MAIN_ACC_pay();
void MENU_MAIN_ACC_to_eco();
void MENU_MAIN_ACC_send_friend();


/*=====================================================================================*/
/* ECO_ACC menus */
void MENU_ECO_ACC_sum();
void MENU_ECO_ACC_to_main();


/*=====================================================================================*/
/* SEND_FRIEND menus */
void MENU_SEND_FRIEND_choose();
void MENU_SEND_FRIEND_no_friend();


/*=====================================================================================*/
/* FRIENDS menus */
void MENU_FRIENDS_see();
void MENU_FRIENDS_add();


/*=====================================================================================*/
/* VIEW_FRIENDS menus */
void MENU_VIEW_FRIENDS_see();
void MENU_VIEW_FRIENDS_no_friend();


/*=====================================================================================*/
/* ADD_FRIENDS menus */
void MENU_ADD_FRIENDS_add();
void MENU_ADD_FRIENDS_no_candidate();


/*=====================================================================================*/
/* NOTIFICATIONS menus */
void MENU_NOTIFICATIONS_see();
void MENU_NOTIFICATIONS_no_new();


/*=====================================================================================*/
/* ENTER_SUM menu */
void MENU_ENTER_sum();


/*=====================================================================================*/
/* TRANSACTION_DONE menu */
void MENU_TRANSACTION_DONE_done(); // can only be reached via Menu::ENTER_SUM


/*=====================================================================================*/
/* NO_FUNDS menu */
void MENU_NO_funds(); // can only be reached via Menu::ENTER_SUM


/*=====================================================================================*/
/* DEBUG menus */
void MENU_DEBUG_wdt();

#endif
