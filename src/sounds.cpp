#include "sounds.h"
#include "pins.h"

void sound_transaction_successful() {
    tone(BUZZER_PIN, 987, 100); // B5
    delay(110);
    tone(BUZZER_PIN, 1175, 100); // D6
    delay(120);
    noTone(BUZZER_PIN);
}


void sound_transaction_failed() {
    tone(BUZZER_PIN, 700, 80);
    delay(100);
    tone(BUZZER_PIN, 500, 150);
    delay(200);
    noTone(BUZZER_PIN);
}
