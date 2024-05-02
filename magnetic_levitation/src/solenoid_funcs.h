#pragma once
#include <Arduino.h>

#include "constants.h"
int most_recent_solenoid_write = 0;

void write_solenoid(int write_value) {
    most_recent_solenoid_write = write_value;
    analogWrite(MOTOR_IN2, write_value);
    if (PRINT_SOLENOID_WRITE_VALUE) {
        Serial.print(">Solenoid value (0-255):");
        Serial.print(String(millis()) + ":");
        Serial.println(write_value);
    }
}

void setup_solenoid() {
    pinMode(MOTOR_IN1, OUTPUT);
    pinMode(MOTOR_IN2, OUTPUT);
    digitalWrite(MOTOR_IN1, LOW);
    write_solenoid(0);
}
