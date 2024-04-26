#include <Arduino.h>

#include "constants.h"
#include "print_statements.h"
#include "sensor_funcs.h"
#include "solenoid_funcs.h"

// timing values
unsigned long startMillis;  // some global variables available anywhere in the program
unsigned long currentMillis;

int between(int lower, int upper, int val);

float steady_state_distance = 8.6;      // millimeters
int steady_state_solenoid_write = 200;  // pwm units

// good lists:
// (Kp, Kd, moving average data points)
// (30, 1, 20)

// float Kp = 30;
// float Kd = Kd / 30;
float Kp = 50;
float Kd = 1.5;
unsigned long last_controller_millis = 0;
float prev_distance_mm = 0;

int controller(float distance_mm) {
    unsigned long current_millis = millis();
    unsigned long dt_milliseconds = current_millis - last_controller_millis;
    last_controller_millis = current_millis;
    float dhalldt_mm_per_s = float(distance_mm - prev_distance_mm) / float(dt_milliseconds * MILLISECONDS_TO_SECONDS);
    int controller_val = between(20, 250, int(Kd * dhalldt_mm_per_s) + int(Kp * (distance_mm - steady_state_distance)) + steady_state_solenoid_write);
    prev_distance_mm = distance_mm;

    if (PRINT_CONTROLLER_VAL) {
        Serial.print(">Controller value (0-255):");
        Serial.print(String(millis()) + ":");
        Serial.println(controller_val);
    }
    return controller_val;
}

int between(int lower, int upper, int val) {
    if (val < lower) {
        return lower;
    }
    if (val > upper) {
        return upper;
    }
    return val;
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
}

void loop() {
    float distance_mm = get_filtered_distance_cm() * 10.0;
    int controller_val = controller(distance_mm);
    write_solenoid(controller_val);
}
