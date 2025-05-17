#include "wdt_counter.h"
#include <Arduino.h>
#include <avr/wdt.h>

// To be incremented by the WDT ISR (private to this file).
static volatile uint16_t wdt_counter = 0;


// WDT Interrupt Service Routine
ISR(WDT_vect) {
    wdt_counter++;
}


void wdt_init() {
    // Temporarily disable interrupts
    cli();

    // CLear Watchdog System Reset Flag
    MCUSR &= ~(1 << WDRF);

    // Set Watchdog Change Enable and Watchdog System Reset Enable
    WDTCSR |= (1 << WDCE) | (1 << WDE);

    // Set interrupt mode and set timeout value to 1s
    WDTCSR = (1 << WDIE) | (1 << WDP2) | (1 << WDP1);

    // Re-enable interrupts.
    sei();
}


uint16_t get_wdt_counter() {
    return wdt_counter;
}
