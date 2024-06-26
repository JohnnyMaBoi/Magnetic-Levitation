#include <Arduino.h>

#include "constants.h"
#include "solenoid_funcs.h"

// Basic sketch for trying out the Adafruit DRV8871 Breakout
// at 13 Ohms
// -3.83V maximum (repeated twice)
// + 3.83V maximum (repeated twice)
// super non-linear though. It's much slower near small signals and changes much more rapidly above ~1V

void setup() {
    Serial.begin(115200);

    Serial.println("");
    Serial.println("DRV8871 test");
    Serial.println("");
    Serial.println("Analog Write Value, Sensor Reading");
    setup_solenoid();
}

void loop() {
    // ramp up forward
    digitalWrite(MOTOR_IN1, LOW);
    for (int i = 0; i < 255; i++) {
        write_solenoid(i);
        Serial.println(String(-i) + ", " + String(analogRead(SENSOR_PIN)));
        delay(100);
    }

    // forward full speed for one second
    delay(1000);

    // ramp down forward
    for (int i = 255; i >= 0; i--) {
        analogWrite(MOTOR_IN2, i);
        Serial.println(String(-i) + ", " + String(analogRead(SENSOR_PIN)));
        delay(100);
    }

    // ramp up backward
    digitalWrite(MOTOR_IN2, LOW);
    for (int i = 0; i < 255; i++) {
        analogWrite(MOTOR_IN1, i);
        Serial.println(String(i) + ", " + String(analogRead(SENSOR_PIN)));
        delay(100);
    }

    // backward full speed for one second
    delay(1000);

    // ramp down backward
    for (int i = 255; i >= 0; i--) {
        analogWrite(MOTOR_IN1, i);
        Serial.println(String(i) + ", " + String(analogRead(SENSOR_PIN)));
        delay(100);
    }
}
