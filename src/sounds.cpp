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


void sound_login_successful() {
    tone(BUZZER_PIN, 1047, 80);   // C6
    delay(100);
    tone(BUZZER_PIN, 1319, 100);  // E6
    delay(120);
    noTone(BUZZER_PIN);
}


void sound_login_failed() {
    tone(BUZZER_PIN, 600, 150);  // D5-ish low tone
    delay(170);
    tone(BUZZER_PIN, 450, 200);  // Slightly lower, “fail” tone
    delay(220);
    noTone(BUZZER_PIN);
}


void sound_logout() {
    tone(BUZZER_PIN, 1175, 120);  // D6 - bright start
    delay(100);
    tone(BUZZER_PIN, 880, 140);   // A5 - smooth descent
    delay(120);
    noTone(BUZZER_PIN);
}


void sound_new_notification() {
    tone(BUZZER_PIN, 880, 120);   // A5 - clear start
    delay(140);
    tone(BUZZER_PIN, 988, 100);   // B5 - sharp follow-up
    delay(120);
    noTone(BUZZER_PIN);
}
