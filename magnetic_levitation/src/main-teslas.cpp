#include <Arduino.h>

#include "constants.h"  // MOTOR_IN1, MOTOR_IN2, SENSOR_PIN
#include "sensor_funcs.h"

// Live plotting of hall in teslas
// No activation of solenoid
// use to sanity check with only hall readings

// Plotting package: TELEPLOT, install with ctrl+p {ext install alexnesnes.teleplot}
// https://marketplace.visualstudio.com/items?itemName=alexnesnes.teleplot

// timing values
unsigned long startMillis;  // some global variables available anywhere in the program
unsigned long currentMillis;

// filtering variables
int prev_minmax_hall = 0;
int minmax_hall = 0;
int moving_avg_hall;

// output variables
float distance_cm;
float mT_out;

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
    hall = analogRead(SENSOR_PIN);
    
    minmax_hall = minmax_filter(hall, prev_minmax_hall);
    prev_minmax_hall = minmax_hall;
    moving_avg_hall = moving_average_filter(minmax_hall);
    
    mT_out = hall_mT(moving_avg_hall);
    distance_cm = mT_to_distance(mT_out);

    Serial.println(String(hall));

    Serial.print(">hall_mT:");
    Serial.print(String(currentMillis) + ":");
    Serial.println(mT_out);

    // Serial.print(">hall_minmax_filtered:");
    // Serial.print(String(currentMillis) + ":");
    // Serial.println(minmax_hall);

    Serial.print(">hall_average_filtered:");
    Serial.print(String(currentMillis) + ":");
    Serial.println(moving_avg_hall);

    Serial.print(">hall_distance:");
    Serial.print(String(currentMillis) + ":");
    Serial.println(distance_cm);

}
