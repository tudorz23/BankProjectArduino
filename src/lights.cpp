#include "lights.h"
#include "pins.h"

void set_color_green() {
    analogWrite(LED_R_PIN, 8);
    analogWrite(LED_G_PIN, 33);
    analogWrite(LED_B_PIN, 2);
}

void set_color_blue() {
    analogWrite(LED_R_PIN, 2);
    analogWrite(LED_G_PIN, 3);
    analogWrite(LED_B_PIN, 30);
}

void set_color_purple() {
    analogWrite(LED_R_PIN, 15);
    analogWrite(LED_G_PIN, 1);
    analogWrite(LED_B_PIN, 25);
}

void set_color_yellow() {
    analogWrite(LED_R_PIN, 30);
    analogWrite(LED_G_PIN, 30);
    analogWrite(LED_B_PIN, 3);
}

void set_color_red() {
    analogWrite(LED_R_PIN, 67);
    analogWrite(LED_G_PIN, 2);
    analogWrite(LED_B_PIN, 2);
}
