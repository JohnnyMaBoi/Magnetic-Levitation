#pragma once

// If you wish, you may redefine these values to be true during your setup function in whatever script you're running.
// Then, each of the functions will print out the correct values.
bool PRINT_RAW_SENSOR_VALUE = false;
bool PRINT_FILTERED_SENSOR_VALUE = false;
bool PRINT_FILTERED_MT = false;
bool PRINT_FILTERED_DISTANCE = false;
bool PRINT_SOLENDOID_CORRECTION_ANALOG = false;
bool PRINT_SOLENOID_CORRECTION_MT = false;
bool PRINT_CONTROLLER_VAL = false;
bool PRINT_CORRECTED_SENSOR_VALUE = false;
bool PRINT_SOLENOID_CORRECTION_LINEAR = false;
bool PRINT_MINMAX_RAILS = false;
bool PRINT_LOOPTIME = false;

// Call this function during setup to turn on all print statemetns
void turn_all_prints_on() {
    PRINT_RAW_SENSOR_VALUE = true;
    PRINT_FILTERED_SENSOR_VALUE = true;
    PRINT_FILTERED_MT = true;
    PRINT_FILTERED_DISTANCE = true;
    PRINT_SOLENDOID_CORRECTION_ANALOG = true;
    PRINT_SOLENOID_CORRECTION_MT = true;
    PRINT_CONTROLLER_VAL = true;
    PRINT_CORRECTED_SENSOR_VALUE = true;
    PRINT_SOLENOID_CORRECTION_LINEAR = true;
    PRINT_LOOPTIME = true;
    PRINT_MINMAX_RAILS = true;
}

// Call this function during setup to turn off all print statemetns
void turn_all_prints_off() {
    PRINT_RAW_SENSOR_VALUE = false;
    PRINT_FILTERED_SENSOR_VALUE = false;
    PRINT_FILTERED_MT = false;
    PRINT_FILTERED_DISTANCE = false;
    PRINT_SOLENDOID_CORRECTION_ANALOG = false;
    PRINT_SOLENOID_CORRECTION_MT = false;
    PRINT_CONTROLLER_VAL = false;
    PRINT_CORRECTED_SENSOR_VALUE = false;
    PRINT_SOLENOID_CORRECTION_LINEAR = false;
    PRINT_LOOPTIME = false;
    PRINT_MINMAX_RAILS = false;
}
