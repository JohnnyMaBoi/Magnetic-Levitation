#pragma once
#include "constants.h"

enum PrintStatements { RAW_SENSOR_VALUE,
                       FILTERED_SENSOR_VALUE,
                       FILTERED_MT,
                       FILTERED_DISTANCE,
                       SOLENDOID_CORRECTION_ANALOG,
                       SOLENOID_CORRECTION_MT };

// HALL CONVERSION AND GLOBAL VAR
// terms from datasheet page 10, sens from table
float temp_effect = 7.5 * (1 + (22 - 25) * .0012);  // assuming 22 deg celsius
int hall_vcc = 5000;                                // running voltage of hall, 5v in mV
float hall_vq = hall_vcc / 2;
int magnet_falloff = 5;  // max sensable distance of magnet

// Hall to Distance Lookups
float distances[18] = {  // closest to farthest
    0.190909092, 0.254545456, 0.31818182,
    0.381818184, 0.445454545, 0.509090912,
    0.572727276, 0.63636364, 0.700000004,
    1, 1.5, 2,
    2.5, 3, 3.5,
    4, 4.5, 5};
float mT[18] = {
    // closest to farthest, magnet-specific
    330,
    275,
    225,
    190,
    160,
    138.5,
    122,
    109.7,
    83,
    37.24,
    18.3,
    9.8,
    5.88,
    3.27,
    1.31,
    .65,
    0,
    -.65,
};

// Convert from analog voltage (0-1023) to magnetic field strength in milli-Teslas
float hall_mT(int hall_analog) {
    // % solve for magnetic flux in units of mT
    // voltages are all in units of mV
    float hall_vout = (hall_analog / 1024.0) * 5000;
    float sensor_flux = (hall_vout - hall_vq) / (temp_effect);
    return sensor_flux;
}

float mT_to_distance(float mT_reading) {
    int i = 0;
    float distance;
    while (mT_reading < mT[i]) {
        i++;
    }
    // mT[i]: lower-bound // mT[i-1]: upper-bound
    float interp_ratio = (mT_reading - mT[i]) / (mT[i - 1] - mT[i]);
    distance = distances[i] + ((distances[i - 1] - distances[i]) * interp_ratio);
    // bounds check
    if (distance < magnet_falloff) {
        return distance;
    }
    return magnet_falloff;
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

int hall;  // analog out
int prev_minmax_hall = 0;
int minmax_hall = 0;
int moving_avg_hall;

int get_raw_sensor_value() {
    return analogRead(SENSOR_PIN);
}

int get_filtered_analog_reading() {
    hall = get_raw_sensor_value();
    minmax_hall = minmax_filter(hall, prev_minmax_hall);
    prev_minmax_hall = minmax_hall;
    moving_avg_hall = moving_average_filter(minmax_hall);
    return moving_avg_hall;
}

float get_filtered_hall_effect_mT() {
    return hall_mT(get_filtered_analog_reading());
}

float get_filtered_distance_cm() {
    return mT_to_distance(get_filtered_hall_effect_mT());
}

// Initialize the moving average array (must be called during setup() function)
void setup_moving_average_array() {
    pinMode(SENSOR_PIN, INPUT);
    // initialize moving average array to all zeros
    for (unsigned int i = 0; i < moving_average_len; i++) {
        moving_average_array[i] = 0;
    }
}

///////////// FUNCTION OVERLOADING SECTION ////////////////////////
// We will take all the functions which normally take x number of arguments and re-define them here to take x+1 arguments,
// where the last argument is a boolean called "print".
// When we call the function with this additional argument set to true, then it will print out the value in addition to calculating it.
// When we call the function with the normal number of arguments, the default function is called as usual.
///////////////////////////////////////////////////////////////////
float get_filtered_hall_effect_mT(bool print) {
    float val = get_filtered_hall_effect_mT();
    Serial.print(">Hall mT (filtered):");
    Serial.print(String(currentMillis) + ":");
    Serial.println(val);
    return val;
}

float get_filtered_distance_cm(bool print) {
    float val = get_filtered_distance_cm();
    Serial.print(">Distance cm (filtered):");
    Serial.print(String(currentMillis) + ":");
    Serial.println(val);
    return val;
}

int get_filtered_analog_reading(bool print) {
    int val = get_filtered_analog_reading();
    Serial.print(">Analog sensor 0-1024 (filtered):");
    Serial.print(String(currentMillis) + ":");
    Serial.println(val);
    return val;
}
