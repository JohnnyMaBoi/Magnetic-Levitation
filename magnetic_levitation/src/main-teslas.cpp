#include <Arduino.h>

#include "constants.h"  // MOTOR_IN1, MOTOR_IN2, SENSOR_PIN
#include "sensor_funcs.h"

// Live plotting of hall in teslas
// No activation of solenoid
// use to sanity check with only hall reading

// Plotting package: TELEPLOT, install with ctrl+p {ext install alexnesnes.teleplot}
// https://marketplace.visualstudio.com/items?itemName=alexnesnes.teleplot

// timing values
unsigned long startMillis;  // some global variables available anywhere in the program
unsigned long currentMillis;

void setup() {
    Serial.begin(115200);

    Serial.println("");
    Serial.println("Sensor Reading in mT and raw analog");

    pinMode(MOTOR_IN1, OUTPUT);
    pinMode(MOTOR_IN2, OUTPUT);
    pinMode(SENSOR_PIN, INPUT);
    analogWrite(MOTOR_IN1, LOW);
    analogWrite(MOTOR_IN2, 200);
    startMillis = millis();  // initial start time
}

void loop() {
    int filtered_analog_reading = get_filtered_analog_reading();

    Serial.println(String(hall));
    // read sensor analog value
    currentMillis = millis();
    // plot processed and unprocessed result
    Serial.print(">hall_analog:");
    Serial.print(String(currentMillis) + ":");
    Serial.println(hall);

    Serial.print(">hall_mT:");
    Serial.print(String(currentMillis) + ":");
    Serial.println(hall_mT(hall));

    Serial.print(">hall_minmax_filtered:");
    Serial.print(String(currentMillis) + ":");
    Serial.println(minmax_hall);

    Serial.print(">hall_average_filtered:");
    Serial.print(String(currentMillis) + ":");
    Serial.println(moving_avg_hall);
}
