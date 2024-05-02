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
int samples_per_interval = 10;

// output variables
int filtered_analog_reading;   // combination average and min-max filter output
int corrected_analog_reading;  // remove solenoid impact from hall analog
float distance_cm;
float mT_out;

void setup() {
    Serial.begin(115200);
    setup_solenoid();

    turn_all_prints_on();
    PRINT_SOLENOID_CORRECTION_FACTOR = true;
    // PRINT_FILTERED_DISTANCE = true;
    PRINT_RAW_SENSOR_VALUE = true;
    PRINT_FILTERED_SENSOR_VALUE = true;
    PRINT_SOLENOID_WRITE_VALUE = true;
}

void loop() {
    write_solenoid(200);
    get_filtered_distance_cm();
}
