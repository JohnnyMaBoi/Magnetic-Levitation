#pragma once

#include "constants.h"
#include "filter_funcs.h"
#include "linear_interp.h"
#include "mT_to_distance_conversion_data.h"
#include "print_statements.h"
#include "solenoid_correction_data.h"
#include "solenoid_funcs.h"

// HALL CONVERSION AND GLOBAL VAR
// terms from datasheet page 10, sens from table
float temp_effect = 7.5 * (1 + (22 - 25) * .0012);  // assuming 22 deg celsius
int hall_vcc = 5000;                                // running voltage of hall, 5v in mV
float hall_vq = hall_vcc / 2;
int magnet_falloff = 5;  // max sensable distance of magnet

// Convert from analog voltage (0-1023) to magnetic field strength in milli-Teslas
float hall_mT(int hall_analog) {
    // % solve for magnetic flux in units of mT
    // voltages are all in units of mV
    float hall_vout = (hall_analog / 1024.0) * 5000;
    float sensor_flux = (hall_vout - hall_vq) / (temp_effect);
    return sensor_flux;
}

// Convert from magnetic field strength reading to distance measurement using a lookup table and linear interpolation
float mT_to_distance(float mT_reading) {
    float distance = linear_interp(mT_reading, mT_to_distance_conversion_mT, mT_to_distance_conversion_distances, n_mT_to_distance_conversion_data_points);

    // bounds check
    if (distance < magnet_falloff) {
        return distance;
    }
    return magnet_falloff;
}

// Takes in the most recent solenoid write value and tries to estimate what the effect of the solenoid will be,
// so that that value can be subtracted out from the sensor reading during get_sensor_value_with_solenoid_subtracted().
int solenoid_correction_func(int solenoid_write_value) {
    // int correction_factor = linear_interp(solenoid_write_value, solenoid_correction_solenoid_write_data_points, solenoid_correction_sensor_read_data_points, n_solenoid_correction_data_points);
    // only valid between 150 and 255
    int correction_factor = 0.5806 * solenoid_write_value + 432;
    // print values
    if (PRINT_SOLENOID_CORRECTION_FACTOR) {
        Serial.print(">solenoid correction (units analog):");
        Serial.print(String(millis()) + ":");
        Serial.println(correction_factor);
    }
    return correction_factor;
}

int get_raw_sensor_value() {
    int val = analogRead(SENSOR_PIN);
    if (PRINT_RAW_SENSOR_VALUE) {
        Serial.print(">analog value (1-1024) (raw):");
        Serial.print(String(millis()) + ":");
        Serial.println(val);
    }
    return val;
}

int get_sensor_value_with_solenoid_subtracted() {
    int solenoid_hall_correction_analog = solenoid_correction_func(most_recent_solenoid_write);
    int val = get_raw_sensor_value() - solenoid_hall_correction_analog + 509;
    if (PRINT_CORRECTED_SENSOR_VALUE) {
        Serial.print(">Hall w solenoid correction (1-1024):");
        Serial.print(String(millis()) + ":");
        Serial.println(val);
    }
    return val;
}

int get_filtered_analog_reading() {
    int raw_val = get_sensor_value_with_solenoid_subtracted();
    int minmax_val = minmax_filter(raw_val);
    int filtered_val = moving_average_filter(minmax_val);
    if (PRINT_FILTERED_SENSOR_VALUE) {
        Serial.print(">analog value (1-1024) (filtered):");
        Serial.print(String(millis()) + ":");
        Serial.println(filtered_val);
    }
    return filtered_val;
}

float get_filtered_hall_effect_mT() {
    float val = hall_mT(get_filtered_analog_reading());
    if (PRINT_FILTERED_MT) {
        Serial.print(">Hall mT (filtered):");
        Serial.print(String(millis()) + ":");
        Serial.println(val);
    }
    return val;
}

float get_filtered_distance_cm() {
    float val = mT_to_distance(get_filtered_hall_effect_mT());
    if (PRINT_FILTERED_DISTANCE) {
        Serial.print(">Distance cm (filtered):");
        Serial.print(String(millis()) + ":");
        Serial.println(val);
    }
    return val;
}

// Initialize the moving average array (must be called during setup() function)
void setup_moving_average_array() {
    pinMode(SENSOR_PIN, INPUT);
    // initialize moving average array to all zeros
    for (unsigned int i = 0; i < moving_average_len; i++) {
        moving_average_array[i] = 0;
    }
}
