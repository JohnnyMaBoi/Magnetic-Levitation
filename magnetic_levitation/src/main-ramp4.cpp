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

void setup() {
    Serial.begin(115200);

    Serial.println("");
    Serial.println("");
    Serial.println("Analog Write Value, Sensor Reading");
    setup_solenoid();

    // turn_all_prints_on();
    // PRINT_FILTERED_DISTANCE = true;
    // PRINT_FILTERED_MT = true;
    // PRINT_RAW_SENSOR_VALUE = true;
}

void loop() {
    // ramp up backward
    digitalWrite(MOTOR_IN2, LOW);
    unsigned long step_duration = 5;  // milliseconds per interval
    unsigned long start_step_time = 0;

    for (int i = 150; i < 255; i++) {
        start_step_time = millis();
        while ((millis() - start_step_time) < step_duration) {
            write_solenoid(i);
            Serial.println(String(i) + "," + String(get_raw_sensor_value()));
            // get_filtered_analog_reading(true);
            delay(1);
        }
    }

    // ramp down backward
    for (int i = 255; i >= 150; i--) {
        start_step_time = millis();
        while ((millis() - start_step_time) < step_duration) {
            write_solenoid(i);
            Serial.println(String(i) + "," + String(get_raw_sensor_value()));
            // get_filtered_analog_reading(true);
            delay(1);
        }
    }
}
