#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2); // I2C address and LCD dimensions

const int mq135Pin = A0;
const int mq6Pin = A1;
const int buzzerPin = 2;

void setup() {
    lcd.init();
    lcd.backlight();
    pinMode(mq135Pin, INPUT);
    pinMode(mq6Pin, INPUT);
    pinMode(buzzerPin, OUTPUT);
}

void loop() {
    int mq135Value = analogRead(mq135Pin);
    int mq6Value = analogRead(mq6Pin);

    float mq135PPM = map(mq135Value, 0, 1023, 0, 1000);
    float mq6PPM = map(mq6Value, 0, 1023, 0, 1000);

    lcd.clear();
    lcd.print("MQ135: " + String(mq135PPM) + " PPM");
    lcd.setCursor(0, 1);
    lcd.print("MQ6: " + String(mq6PPM) + " PPM");

    if (mq135PPM > 200 || mq6PPM > 100) {
        digitalWrite(buzzerPin, HIGH); // Activate the buzzer
        // Log data (timestamp, PPM values, alarm status)
    } else {
        digitalWrite(buzzerPin, LOW); // Deactivate the buzzer
    }

    delay(1000); // Update every second
}
