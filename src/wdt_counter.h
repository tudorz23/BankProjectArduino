#ifndef _WDT_COUNTER_MODULE_
#define _WDT_COUNTER_MODULE_

#include <stdint.h>

// Setup the Watchdog timer to generate an interrupt every 1 second,
// to manage an interest gain in the economy accounts.
void wdt_init();

// Getter for the wdt_counter value.
uint16_t get_wdt_counter();

#endif
