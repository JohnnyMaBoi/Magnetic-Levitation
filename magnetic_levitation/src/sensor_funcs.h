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
    return distance;
}

// Takes in the most recent solenoid write value and tries to estimate what the effect of the solenoid will be,
// so that that value can be subtracted out from the sensor reading during get_sensor_value_with_solenoid_subtracted().
float solenoid_correction_linear() {
    // int correction_factor = linear_interp(solenoid_write_value, solenoid_correction_solenoid_write_data_points, solenoid_correction_sensor_read_data_points, n_solenoid_correction_data_points);
    // only valid between 150 and 255
    float correction_factor = 0;
    if (most_recent_solenoid_write >= 155) {
        correction_factor = 0.5806 * most_recent_solenoid_write + 432 - 509;
    } else if ((most_recent_solenoid_write < 155)) {  // && (solenoid_write_value > 50)
        correction_factor = 0.0952 * most_recent_solenoid_write + 505 - 509;
    }
    // print values
    if (PRINT_SOLENOID_CORRECTION_LINEAR) {
        Serial.print(">solenoid equation correction:");
        Serial.print(String(millis()) + ":");
        Serial.println(correction_factor);
    }
    return correction_factor;
}

int solenoid_correction_interp() {
    float correction_factor = linear_interp(most_recent_solenoid_write, solenoid_correction_solenoid_write_data_points, solenoid_correction_sensor_read_data_points, n_solenoid_correction_data_points);
    return correction_factor;
}

// Basically analogRead(), except it also prints the value if the print function is enabled.

int get_raw_sensor_value() {
    // int val = analogRead(SENSOR_PIN);
    int val = ADCH;
    if (PRINT_RAW_SENSOR_VALUE) {
        Serial.print(">Analog Read (raw):");
        Serial.print(String(millis()) + ":");
        Serial.println(val);
    }
    return val;
}

// Access the global variable that is set every time the solenoid is written to to figure out the current solenoid value.
// Then, call a function to calculate the expected magnetic field strength due to the solenoid.
// Then, subtract out the solenoid's magnetic field to get just the magnetic field from the magnet.
int get_sensor_value_with_solenoid_subtracted() {
    int solenoid_hall_correction_analog = solenoid_correction_linear();
    int val = get_raw_sensor_value() - solenoid_hall_correction_analog;
    if (PRINT_CORRECTED_SENSOR_VALUE) {
        Serial.print(">Hall w solenoid correction (1-1024):");
        Serial.print(String(millis()) + ":");
        Serial.println(val);
    }
    return val;
}

// Apply either just the minmax filter, or both minmax and a moving average filter.
// This function takes an argument to determine whether the moving average filter should be applied.

int apply_filter(int raw_val, bool apply_moving_average) {
    // int minmax_val = minmax_filter(raw_val);
    int minmax_val = raw_val;

    int filtered_val = 0;
    // add the option to skip the moving average filter step
    if (apply_moving_average) {
        filtered_val = moving_average_filter(minmax_val);
    } else {
        filtered_val = minmax_val;
    }
    if (PRINT_FILTERED_SENSOR_VALUE) {
        Serial.print(">Filtered Analog Read (0-1024)");
        if (apply_moving_average) {
            Serial.print(":");
        } else {
            Serial.print(" (no mov.avg.):");
        }
        Serial.print(String(millis()) + ":");
        Serial.println(filtered_val);
    }
    return filtered_val;
}

// defining analog reading variables for use in scripts
int raw_val;                       // hall analog reading
int filtered_val = 0;              // filtered analog reading
int prev_filtered_val;             // filtered analog reading
float eq_solenoid_correction;      // calculated solenoid correction from best fit
float interp_solenoid_correction;  // interp solenoid correction datapoints
float eq_solenoid;
float prev_filtered_solenoid;
float filtered_solenoid;
float eq_mT;
float mT;
float eq_distance;
float distance_cm;
// timing variables
unsigned int end_timer;
unsigned int start_timer;

void get_filtered_analog_reading(bool apply_moving_average) {
    // move around start/end to time loop elements
    prev_filtered_val = filtered_val;
    prev_filtered_solenoid = filtered_solenoid;
    raw_val = get_raw_sensor_value();
    interp_solenoid_correction = solenoid_correction_interp();
    filtered_solenoid = raw_val - interp_solenoid_correction;
    filtered_val = apply_filter(filtered_solenoid, apply_moving_average);
}
