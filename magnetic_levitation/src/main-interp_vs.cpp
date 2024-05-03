#include <Arduino.h>

#include "constants.h"
#include "sensor_funcs.h"
#include "solenoid_funcs.h"

// Basic sketch for trying out the Adafruit DRV8871 Breakout
// at 13 Ohms
// -3.83V maximum (repeated twice)
// + 3.83V maximum (repeated twice)
// super non-linear though. It's much slower near small signals and changes much more rapidly above ~1V

// timing values
unsigned long startMillis;  // some global variables available anywhere in the program
unsigned long currentMillis;
unsigned long sample_time = 1000;      // time for each sample in ms
int samples_per_interval = 10;




void setup() {
    Serial.begin(115200);

    Serial.println("");
    Serial.println("DRV8871 test");
    Serial.println("");
    Serial.println("Analog Write Value, Sensor Reading");
    setup_solenoid();
    // turn_all_prints_on();
    turn_all_prints_off();
    // PRINT_FILTERED_DISTANCE = true;
    // PRINT_FILTERED_MT = true;
    PRINT_RAW_SENSOR_VALUE = true;
    PRINT_FILTERED_MT = true; 
    // PRINT_FILTERED_SENSOR_VALUE = true;
}
// defining equation hall correction variables
float eq_solenoid_correction; // calculated solenoid correction from best fit
float eq_solenoid;
float eq_mT;
float eq_distance;

int get_eq_values() {
    start_timer = millis();
    eq_solenoid_correction = solenoid_correction_linear();
    eq_solenoid = filtered_val-eq_solenoid_correction;
    eq_mT = hall_mT(eq_solenoid);
    eq_distance = mT_to_distance(eq_mT);
    end_timer = millis();

    // make sure to change labels when you switch out testing values
    Serial.print(">eq mT:"+String(end_timer) + ":");
    Serial.println(eq_distance);

    Serial.print(">interp mT:"+String(end_timer) + ":");
    Serial.println(distance_cm);

    Serial.print(">loop time:"+String(end_timer) + ":");
    Serial.println(end_timer-start_timer);
}


void loop() {
    // ramp up backward
    digitalWrite(MOTOR_IN2, LOW);
    for (int i = 0; i < 255 * samples_per_interval; i++) {
        write_solenoid(i / samples_per_interval);
        startMillis = millis();
        currentMillis = millis();
        while((startMillis - currentMillis)<sample_time){
            currentMillis = millis();
            get_filtered_analog_reading(true);
            get_eq_values();
            Serial.print(">Solenoid value (0-255):");
            Serial.print(String(millis()) + ":");
            Serial.println(i / samples_per_interval);
        }
    }

    // ramp down backward
    for (int i = 255 * samples_per_interval; i >= 0; i--) {
        write_solenoid(i / samples_per_interval);
        startMillis = millis();
        currentMillis = millis();
        while((startMillis - currentMillis)<sample_time){
            currentMillis = millis();
            get_filtered_analog_reading(true);
            Serial.print(">Solenoid value (0-255):");
            Serial.print(String(millis()) + ":");
            Serial.println(i / samples_per_interval);
        }
    }
}
