#include <Arduino.h>

#include "constants.h"
int most_recent_solenoid_write = 0;
int solenoid_hall_correction_analog = 0;

void write_solenoid(int write_value) {
    most_recent_solenoid_write = write_value;
    float float_write_value = float(write_value);
    solenoid_hall_correction_analog = int((float_write_value / 62.5) * (float_write_value / 62.5) * (float_write_value / 62.5) - float_write_value * 0.015);
    digitalWrite(MOTOR_IN1, LOW);
    analogWrite(MOTOR_IN2, write_value);
}

void setup_solenoid() {
    pinMode(MOTOR_IN1, OUTPUT);
    pinMode(MOTOR_IN2, OUTPUT);
    write_solenoid(0);
}
