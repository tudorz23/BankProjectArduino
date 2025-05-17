#ifndef _PINS_H_
#define _PINS_H_

#include <Arduino.h>

/* COMPONENT PINS */
// Card reader
constexpr uint8_t RFID_SDA_PIN = 8;
constexpr uint8_t RFID_RST_PIN = 7;

// Joystick
constexpr uint8_t JOYSTICK_VRX_PIN = A0;
constexpr uint8_t JOYSTICK_VRY_PIN = A1;
constexpr uint8_t JOYSTICK_SW_PIN = A2;

// TTP229 keyboard
constexpr uint8_t TTP_SCL_PIN = 4;
constexpr uint8_t TTP_SDO_PIN = 2;

// RGB led
constexpr uint8_t LED_R_PIN = 10;
constexpr uint8_t LED_G_PIN = 9;
constexpr uint8_t LED_B_PIN = 6;

// Buzzer
constexpr uint8_t BUZZER_PIN = 3;

// Red button
constexpr uint8_t RED_BUTTON_PIN = 5;

#endif
