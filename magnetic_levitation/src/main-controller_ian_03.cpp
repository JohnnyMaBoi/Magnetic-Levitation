#include <Arduino.h>

#include "constants.h"
#include "print_statements.h"
#include "sensor_funcs.h"
#include "solenoid_funcs.h"

// timing values
unsigned long startMillis = 0;  // some global variables available anywhere in the program
unsigned long prevMillis = 0;
unsigned long prevMillisNLoops = 0;
unsigned long currentMicros;
unsigned long looptime_ms;
unsigned long looptime_NLoops;

void adc_setup();

void updateTimers() {
    currentMicros = micros();
    looptime_ms = currentMicros - prevMillis;
    prevMillis = currentMicros;
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

// good lists:
// (Kp, Kd, moving average data points, steady_state_distance, steady_state_solenoid)
// (30, 1, 20, 8.6, 200)
// (10, 0.1, 10, 13, 200)
// (2, 0.1, 0/20, 110, 200)

// float Kp = 30;
// float Kd = Kd / 30;
// float Kp = 50;
// float Kd = 1.5;

// setpoint
int steady_state_analog_read = -73;
int steady_state_solenoid_write = 200;  // pwm units
float Kp = -2.9 * 1;
float Kd = -0.09 / 1000 / 2;
int controller_val;  // val to be written to solenoid

void controller() {
    // take reading
    int Kd_term = -Kd * (filtered_solenoid - prev_filtered_solenoid) / float(looptime_ms * MICROSECONDS_TO_MILLISECONDS * MILLISECONDS_TO_SECONDS);
    int Kp_term = -Kp * (filtered_solenoid - steady_state_analog_read);
    controller_val = between(150, 250, Kd_term + Kp_term + steady_state_solenoid_write);
    if (PRINT_CONTROLLER_VAL) {
        Serial.println(">Kd term:" + String(currentMicros * MICROSECONDS_TO_MILLISECONDS) + ":" + Kd_term);
        Serial.println(">Kp term:" + String(currentMicros * MICROSECONDS_TO_MILLISECONDS) + ":" + Kp_term);
        Serial.println(">Controller val:" + String(currentMicros * MICROSECONDS_TO_MILLISECONDS) + ":" + controller_val);
    }
}

void setup() {
    Serial.begin(115200);
    Serial.println("");
    Serial.println("Hello, World!");
    Serial.println("");
    setup_moving_average_array();
    setup_solenoid();
    startMillis = micros();  // initial start time
    // Serial.println("Starting ADC setup");
    // adc_setup();
    // Serial.println("Finished ADC setup");
}

void turn_on_specific_print_statements() {
    // turn_all_prints_on();
    // PRINT_CONTROLLER_VAL = true;
    // PRINT_FILTERED_SENSOR_VALUE = true;
    // PRINT_LOOPTIME = true;
    // PRINT_RAW_SENSOR_VALUE = true;
}

#define N_LOOP_CYCLES_BEFORE_PRINT 999
void loop() {
    for (int loop_cycle = 0; loop_cycle < N_LOOP_CYCLES_BEFORE_PRINT; loop_cycle++) {
        get_filtered_analog_reading(true);
        updateTimers();
        controller();
        write_solenoid(controller_val);
        // write_solenoid(200);
        if (loop_cycle == 0) {
            turn_on_specific_print_statements();

            looptime_NLoops = (micros() - prevMillisNLoops) / N_LOOP_CYCLES_BEFORE_PRINT;
            prevMillisNLoops = micros();
            if (PRINT_LOOPTIME) {
                Serial.println(">Loop time:" + String(millis()) + ":" + looptime_NLoops);  // print out the most recent loop time
            }
        } else if (loop_cycle == 1) {
            turn_all_prints_off();
        }
    }
}

// void adc_setup() {
//     // cli();
//     ADCSRA = 0b11110100;
//     ADCSRB = 0b00000000;
//     ADMUX = 0b01100000;
//     // sei();
// }
