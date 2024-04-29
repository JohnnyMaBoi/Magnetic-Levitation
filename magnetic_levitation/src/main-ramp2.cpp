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

// output variables
int filtered_analog_reading;   // combination average and min-max filter output
int corrected_analog_reading;  // remove solenoid impact from hall analog
float distance_cm;
float mT_out;

void setup() {
    Serial.begin(115200);

    Serial.println("");
    Serial.println("DRV8871 test");
    Serial.println("");
    Serial.println("Analog Write Value, Sensor Reading");
    setup_solenoid();
    turn_all_prints_off();
    turn_all_prints_on();
    // PRINT_FILTERED_DISTANCE = true;
    // PRINT_FILTERED_MT = true;
    PRINT_RAW_SENSOR_VALUE = true;
    // PRINT_FILTERED_SENSOR_VALUE = true;
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
            get_filtered_distance_cm();
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
            get_filtered_distance_cm();
            Serial.print(">Solenoid value (0-255):");
            Serial.print(String(millis()) + ":");
            Serial.println(i / samples_per_interval);
        }
    }
}
