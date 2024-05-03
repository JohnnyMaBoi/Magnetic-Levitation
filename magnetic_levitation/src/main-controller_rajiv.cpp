#include <Arduino.h>

#include "constants.h"
#include "print_statements.h"
#include "sensor_funcs.h"
#include "solenoid_funcs.h"

// timing values
unsigned long startMillis;  // some global variables available anywhere in the program
unsigned long currentMillis;

// good lists:
// (Kp, Kd, moving average data points, steady_state_distance, steady_state_solenoid)
// (30, 1, 20, 8.6, 200)
// (10, 0.1, 10, 13, 200)

// float Kp = 30;
// float Kd = Kd / 30;
// float Kp = 50;
// float Kd = 1.5;
unsigned long looptime_ms = 3;
float prev_distance_mm = 0;
unsigned long current_millis = millis();
unsigned long dt_milliseconds;

int between(float lower, float upper, float val) {
    if (val < lower) {
        return (int)lower;
    }
    if (val > upper) {
        return (int)upper;
    }
    return (int)val;
}

// setpoint
float setpoint = 1.25;                  // cm
int steady_state_solenoid_write = 200;  // pwm units
float last_distance;
float Kp = 150;
float Kd = 0.1;
int controller_val;  // val to be written to solenoid
float error = 5 - setpoint;
float last_error = error;
float derror;
float derror_threshold = .05;

void controller() {
    last_error = error;
    // take reading
    get_filtered_analog_reading(false);
    looptime_ms = millis() - current_millis;
    current_millis = millis();
    error = distance_cm - setpoint;
    // derror = error-last_error;
    // if (derror>derror_threshold){
    //     derror = (derror*1000.0) / (looptime_ms);
    //     controller_val = between(0.0, 255.0, Kp*error+ (derror*1000.0) /(looptime_ms)+steady_state_solenoid_write);

    // }
    // else{
    controller_val = between(0.0, 255.0, Kp * error + steady_state_solenoid_write);
    // }
    // if (error>0){
    //     controller_val = 255;
    // }
    // else{
    //     controller_val = 100;
    // }
    // int controller_val = between(150, 250, int(Kd * dhalldt_mm_per_s) + steady_state_solenoid_write);

    // if (PRINT_CONTROLLER_VAL) {
    Serial.println(">Controller value (0-255):" + String(millis()) + ":" + controller_val);
    // }
    // if (PRINT_LOOPTIME) {
    // Serial.println(">distance:"+String(millis())+":"+distance_cm);
    Serial.println(">error:" + String(millis()) + ":" + error);

    // }
}

void setup() {
    Serial.begin(115200);
    setup_moving_average_array();
    setup_solenoid();
    startMillis = millis();  // initial start time

    // setup print statements
    // turn_all_prints_on();
    turn_all_prints_off();
    PRINT_CONTROLLER_VAL = true;
    // PRINT_FILTERED_DISTANCE = true;
    // PRINT_FILTERED_SENSOR_VALUE = true;
}

void loop() {
    write_solenoid(controller_val);
    controller();
}
