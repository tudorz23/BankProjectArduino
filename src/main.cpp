#include <Arduino.h>
#include <MFRC522.h>
#include <SPI.h>
#include <ezButton.h>
#include <LiquidCrystal_I2C.h>
#include <TTP229.h>


const int BUTTON_PIN = A3;

const int RFID_SDA_PIN = 8;
const int RFID_RST_PIN = 7;

const int JOYSTICK_VRX_PIN = A0;
const int JOYSTICK_VRY_PIN = A1;
const int JOYSTICK_SW_PIN = A2;

const int TTP_SCL_PIN = 4; // The pin number of the clock pin.
const int TTP_SDO_PIN = 2; // The pin number of the data pin.

const int LED_R_PIN = 10;
const int LED_G_PIN = 9;
const int LED_B_PIN = 6;

const int CANCEL_BUTTON_PIN = 5;

const int BUZZER_PIN = 3;


/* OBJECTS */
TTP229 ttp229(TTP_SCL_PIN, TTP_SDO_PIN); // TTP229(sclPin, sdoPin)

MFRC522 mfrc522(RFID_SDA_PIN, RFID_RST_PIN);

ezButton button(JOYSTICK_SW_PIN);

LiquidCrystal_I2C lcd(0x27,16,2);  // set the LCD address to 0x27 for a 16 chars and 2 line display

ezButton cancelButton(CANCEL_BUTTON_PIN, EXTERNAL_PULLUP);

void setup_rfid() {
    SPI.begin();

    mfrc522.PCD_Init();

    Serial.println("Approximate your card to the reader...");
    Serial.println();
}

#define LEFT_THRESHOLD  400
#define RIGHT_THRESHOLD 800
#define UP_THRESHOLD    400
#define DOWN_THRESHOLD  800

#define COMMAND_NO     0x00
#define COMMAND_LEFT   0x01
#define COMMAND_RIGHT  0x02
#define COMMAND_UP     0x04
#define COMMAND_DOWN   0x08

int command = COMMAND_NO;

int xValue = 0;
int yValue = 0;
int bValue = 0;
void setup_joystick() {
    button.setDebounceTime(100);
}

void setup_led() {
    pinMode(LED_R_PIN,   OUTPUT);
    pinMode(LED_G_PIN, OUTPUT);
    pinMode(LED_B_PIN,  OUTPUT);

    analogWrite(LED_R_PIN, 150);
    analogWrite(LED_G_PIN, 0);
    analogWrite(LED_B_PIN, 0);
}

void setup() {
    // USART communication at 9600 baud.
    Serial.begin(9600);

    lcd.init();
	lcd.clear();         
	lcd.backlight();      // Make sure backlight is on

    setup_rfid();
    setup_joystick();

    setup_led();

    lcd.print(F("Scuderia Ferrari"));

    cancelButton.setDebounceTime(100);
    // pinMode(CANCEL_BUTTON_PIN, INPUT);

    pinMode(BUZZER_PIN, OUTPUT);
    // tone(BUZZER_PIN, 1000, 2000);
}


void test_rfid() {
    while (true) {
        // Serial.print("Here\n");
        // Look for new cards
        if (!mfrc522.PICC_IsNewCardPresent()) 
        {
            continue;
        }

        // Select one of the cards
        if ( !mfrc522.PICC_ReadCardSerial()) 
        {
            Serial.println("Error\n");
            continue;
        }

        //Show UID on serial monitor
        Serial.print("UID tag :");
        String content = "";
        byte letter;

        for (byte i = 0; i < mfrc522.uid.size; i++) 
        {
            Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
            Serial.print(mfrc522.uid.uidByte[i], HEX);
            content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
            content.concat(String(mfrc522.uid.uidByte[i], HEX));
        }

        Serial.println();
        // Serial.print("Message : ");
        // content.toUpperCase();

        // if (content.substring(1) == "BD 31 15 2B") //change here the UID of the card/cards that you want to give access
        // {
        //     Serial.println("Authorized access");
        //     Serial.println();
        //     delay(3000);
        // }
        
        // else {
        //     Serial.println(" Access denied");
        //     delay(3000);
        // }
    }
}

void test_joystick() {
    while (true) {
        button.loop(); // MUST call the loop() function first

        // read analog X and Y analog values
        xValue = analogRead(JOYSTICK_VRX_PIN);
        yValue = analogRead(JOYSTICK_VRY_PIN);

        // converts the analog value to commands
        // reset commands
        command = COMMAND_NO;

        // check left/right commands
        if (xValue < LEFT_THRESHOLD)
            command = command | COMMAND_LEFT;
        else if (xValue > RIGHT_THRESHOLD)
            command = command | COMMAND_RIGHT;

        // check up/down commands
        if (yValue < UP_THRESHOLD)
            command = command | COMMAND_UP;
        else if (yValue > DOWN_THRESHOLD)
            command = command | COMMAND_DOWN;

        // NOTE: AT A TIME, THERE MAY BE NO COMMAND, ONE COMMAND OR TWO COMMANDS

        // print command to serial and process command
        if (command & COMMAND_LEFT) {
            Serial.println("COMMAND LEFT");
            // TODO: add your task here
        }

        if (command & COMMAND_RIGHT) {
            Serial.println("COMMAND RIGHT");
            // TODO: add your task here
        }

        if (command & COMMAND_UP) {
            Serial.println("COMMAND UP");
            // TODO: add your task here
        }

        if (command & COMMAND_DOWN) {
            Serial.println("COMMAND DOWN");
            // TODO: add your task here
        }

        // Read the button value
        bValue = button.getState();

        if (button.isPressed()) {
            Serial.println(F("The button is pressed"));
            // TODO do something here
        }

        if (button.isReleased()) {
            Serial.println(F("The button is released"));
            // TODO do something here
        }

        // print data to Serial Monitor on Arduino IDE
        // Serial.print(F("x = "));
        // Serial.print(xValue);
        // Serial.print(F(", y = "));
        // Serial.print(yValue);
        // Serial.print(F(" : button = "));
        // Serial.println(bValue);
        // delay(200);
    }
}

void test_ttp() {
    while (true) {
        int res = ttp229.ReadKey16();

        Serial.println(res);
    }
}

void test_led() {
    while (true) {
        // color code #00C9CC (R = 0,   G = 201, B = 204)
        analogWrite(LED_R_PIN,   0);
        analogWrite(LED_G_PIN, 201);
        analogWrite(LED_B_PIN,  0);

        delay(1000); // keep the color 1 second

        // color code #F7788A (R = 247, G = 120, B = 138)
        analogWrite(LED_R_PIN,   247);
        analogWrite(LED_G_PIN, 0);
        analogWrite(LED_B_PIN,  0);

        delay(1000); // keep the color 1 second

        // color code #34A853 (R = 52,  G = 168, B = 83)
        analogWrite(LED_R_PIN,   0);
        analogWrite(LED_G_PIN, 0);
        analogWrite(LED_B_PIN,  230);

        delay(1000); // keep the color 1 second
    }
}

void test_cancel_button() {
    while (true) {
        cancelButton.loop();

        int val = cancelButton.getState();

        if (cancelButton.isPressed()) {
            Serial.println(F("The red button is pressed"));
        }

        if (cancelButton.isReleased()) {
            Serial.println(F("The red button is released"));
            // TODO do something here
        }
        // int val = digitalRead(CANCEL_BUTTON_PIN);

        // if (val == HIGH) {
        //     Serial.println(F("The red button is released"));
        // } else {
        //     Serial.println(F("The red button is pressed"));
        // }
    }
}

void test_buzzer() {
    while (true) {
        tone(BUZZER_PIN, 440); // A4
        delay(1000);

        tone(BUZZER_PIN, 494); // B4
        delay(1000);

        tone(BUZZER_PIN, 523); // C4
        delay(1000);

        tone(BUZZER_PIN, 587); // D4
        delay(1000);

        tone(BUZZER_PIN, 659); // E4
        delay(1000);

        tone(BUZZER_PIN, 698); // F4
        delay(1000);

        tone(BUZZER_PIN, 784); // G4
        delay(1000);

        noTone(BUZZER_PIN);
        delay(1000);
    }
}

void loop() {

    // test_rfid();
    test_joystick();
    // test_ttp();

    // test_led();

    // test_cancel_button();

    // test_buzzer();

    delay(500);
}
