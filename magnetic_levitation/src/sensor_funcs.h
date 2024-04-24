#include <Arduino.h>

#include "constants.h"

// HALL CONVERSION AND GLOBAL VAR
// %terms from datasheet page 10, sens from table
// float Vq = 3.3/2;
// float sensitivity_term = 7.5; // changes by device & voltage
// float temp_effect = sensitivity_term*(1+(22-25)*.0012); // assuming 22 deg celsius
float temp_effect = 7.5 * (1 + (22 - 25) * .0012);  // assuming 22 deg celsius
int hall_vcc = 5000;                                // running voltage of hall, 5v in mV
float hall_vq = hall_vcc / 2;
int hall;  // analog output variable

// Convert from analog voltage (0-1023) to magnetic field strength in milli-Teslas
float hall_mT(int hall_analog) {
    // % solve for magnetic flux in units of mT
    // voltages are all in units of mV
    float hall_vout = (hall_analog / 1024.0) * 5000;
    float sensor_flux = (hall_vout - hall_vq) / (temp_effect);
    return sensor_flux;
}

// Perform moving average filter
const unsigned int moving_average_len = 3;
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
const unsigned int moving_minmax_len = 20;
const float buffer_factor = -0.5;  // -0.5;
float moving_minmax_array[moving_minmax_len];

int minmax_filter(int new_val, int prev_val) {
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

    moving_minmax_array[0] = new_val;
    if ((new_val > max_val_overall) || (new_val < min_val_overall)) {
        return (max_val + min_val) / 2;
    } else {
        return prev_val;
    }
}

// Initialize the moving average array (must be called during setup() function)
void setup_moving_average_array() {
    pinMode(SENSOR_PIN, INPUT);
    // initialize moving average array to all zeros
    for (unsigned int i = 0; i < moving_average_len; i++) {
        moving_average_array[i] = 0;
    }
}

int prev_minmax_hall = 0;
int minmax_hall = 0;
int moving_avg_hall;

int get_filtered_analog_reading() {
    hall = analogRead(SENSOR_PIN);
    minmax_hall = minmax_filter(hall, prev_minmax_hall);
    prev_minmax_hall = minmax_hall;
    moving_avg_hall = moving_average_filter(minmax_hall);
    return moving_avg_hall;
}

float get_filtered_hall_mT() {
    return hall_mT(get_filtered_analog_reading());
}
