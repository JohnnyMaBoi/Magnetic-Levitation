#include <Arduino.h>

#include "constants.h"
#include "print_statements.h"
#include "sensor_funcs.h"
#include "solenoid_funcs.h"

// timing values
unsigned long startMillis;  // some global variables available anywhere in the program
unsigned long currentMillis;

int between(int lower, int upper, int val) {
    if (val < lower) {
        return lower;
    }
    if (val > upper) {
        return upper;
    }
    return val;
}

unsigned long last_controller_millis = 0;
float prev_analog_reading = 0;
float prev_Kd_value = 0;

// good lists:
// (Kp, Kd, moving average data points, steady_state_distance, steady_state_solenoid)
// (30, 1, 20, 8.6, 200)
// (10, 0.1, 10, 13, 200)
// (0.5, 0.01, 10, 520, 200)
// (1.5, 0.03, 5, 550, 200)

// float Kp = 30;
// float Kd = Kd / 30;
// float Kp = 50;
// float Kd = 1.5;
// float Kp = 160;
// float Kd = 0.1;

float steady_state_analog_reading = 550;  // analog reading
int steady_state_solenoid_write = 200;    // pwm units
float Kp = 0.5 * 3;
float Kd = 0.01 * 3;

float gamma = 40;
float prev_Kd_value_factor = (gamma * Kd - 1) / (gamma * Kd);

int controller(int analog_reading) {
    unsigned long current_millis = millis();
    unsigned long dt_milliseconds = current_millis - last_controller_millis;
    last_controller_millis = current_millis;
    float dhalldt_mm_per_s = float(analog_reading - prev_analog_reading) / float(dt_milliseconds * MILLISECONDS_TO_SECONDS);
    int Kp_value = int(-Kp * (analog_reading - steady_state_analog_reading));
    int Kd_value = int(prev_Kd_value_factor * prev_Kd_value + dhalldt_mm_per_s * 1 / gamma);
    int controller_val = between(150, 250, Kd_value + Kp_value + steady_state_solenoid_write);
    // int controller_val = between(150, 250, int(Kd * dhalldt_mm_per_s) + steady_state_solenoid_write);
    prev_analog_reading = analog_reading;

    if (PRINT_CONTROLLER_VAL) {
        Serial.print(">Raw Kp value:");
        Serial.print(String(millis()) + ":");
        Serial.println(Kp_value);
    }
    if (PRINT_CONTROLLER_VAL) {
        Serial.print(">Raw Kd value (0-255):");
        Serial.print(String(millis()) + ":");
        Serial.println(Kd_value);
    }
    return controller_val;
}

void setup() {
    Serial.begin(115200);
    setup_moving_average_array();
    setup_solenoid();
    startMillis = millis();  // initial start time

    // setup print statements
    // turn_all_prints_on();
    turn_all_prints_off();
    PRINT_SOLENOID_WRITE_VALUE = true;
    PRINT_FILTERED_SENSOR_VALUE = true;
    PRINT_CONTROLLER_VAL = true;
}

void loop() {
    // Serial.print("Loop time (ms): ");
    // unsigned long loop_start_time = millis();
    // float distance_mm_for_p = get_filtered_distance_cm(false) * 10.0;
    int analog_reading_filtered = get_filtered_analog_reading(true);
    int controller_val = controller(analog_reading_filtered);
    write_solenoid(controller_val);
    // unsigned long loop_end_time = millis();
    // Serial.print(loop_start_time);
    // Serial.print(" - ");
    // Serial.println(loop_end_time);
}
