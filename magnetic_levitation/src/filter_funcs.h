#pragma once
#include <Arduino.h>

#include "filter_funcs.h"
#include "print_statements.h"
#include "solenoid_funcs.h"

int solenoid_correction_interp() {
    // float correction_factor = linear_interp(most_recent_solenoid_write, solenoid_correction_solenoid_write_data_points, solenoid_correction_sensor_read_data_points, n_solenoid_correction_data_points);
    int correction_factor = most_recent_solenoid_write * -0.8932 + 904;
    if (PRINT_SOLENDOID_CORRECTION_ANALOG) {
        Serial.print(">solenoid correction:");
        Serial.print(String(millis()) + ":");
        Serial.println(correction_factor);
    }
    return correction_factor;
}

// Perform moving average filter
const unsigned int moving_average_len = 5;
int moving_average_array[moving_average_len];

int moving_average_filter(int new_val) {
    int moving_average = new_val;
    for (unsigned int i = 0; i < moving_average_len - 1; i++) {
        moving_average += moving_average_array[moving_average_len - i - 1];
        moving_average_array[moving_average_len - i - 1] = moving_average_array[moving_average_len - i - 2];
    }
    moving_average_array[0] = new_val;
    return moving_average / moving_average_len;
}

// Perform min-max filter
const unsigned int moving_minmax_len = 5;
float buffer_factor = -0.2;
float moving_minmax_array[moving_minmax_len];
int prev_minmax_val = 0;

int minmax_filter(int new_val) {
    int max_val = -10000;
    int min_val = 10000;
    for (unsigned int i = 0; i < moving_minmax_len - 1; i++) {
        max_val = max(max_val, moving_minmax_array[moving_minmax_len - i - 1]);
        min_val = min(min_val, moving_minmax_array[moving_minmax_len - i - 1]);
        moving_minmax_array[moving_minmax_len - i - 1] = moving_minmax_array[moving_minmax_len - i - 2];
    }
    int minmax_diff = max_val - min_val;
    int max_val_overall = max_val + minmax_diff * buffer_factor;
    int min_val_overall = min_val - minmax_diff * buffer_factor;

    if (PRINT_MINMAX_RAILS) {
        Serial.println(">min_val_overall:" + String(millis()) + ":" + min_val_overall);
        Serial.println(">max_val_overall:" + String(millis()) + ":" + max_val_overall);
    }

    moving_minmax_array[0] = new_val;
    if ((new_val > max_val_overall) || (new_val < min_val_overall)) {
        prev_minmax_val = (max_val + min_val) / 2;
        return (max_val + min_val) / 2;
    } else {
        return prev_minmax_val;
    }
}

// Initialize the moving average array (must be called during setup() function)
void setup_moving_average_array() {
    pinMode(SENSOR_PIN, INPUT);
    // initialize moving average array to all zeros
    for (unsigned int i = 0; i < moving_average_len; i++) {
        moving_average_array[i] = 0;
    }
    for (unsigned int i = 0; i < moving_minmax_len; i++) {
        moving_minmax_array[i] = 0;
    }
}
