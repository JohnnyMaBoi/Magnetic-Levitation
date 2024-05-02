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
    int correction_factor = 0;
    if (solenoid_write_value >= 150) {
        correction_factor = 0.5806 * solenoid_write_value + 432;
    } else if ((solenoid_write_value > 50) && (solenoid_write_value < 150)) {
        correction_factor = 0.0952 * solenoid_write_value + 505;
    }
    // print values
    if (PRINT_SOLENOID_CORRECTION_FACTOR) {
        Serial.print(">solenoid correction (units analog):");
        Serial.print(String(millis()) + ":");
        Serial.println(correction_factor);
    }
    return correction_factor;
}

// Basically analogRead(), except it also prints the value if the print function is enabled.
int get_raw_sensor_value() {
    int val = analogRead(SENSOR_PIN);
    if (PRINT_RAW_SENSOR_VALUE) {
        Serial.print(">analog value (1-1024) (raw):");
        Serial.print(String(millis()) + ":");
        Serial.println(val);
    }
    return val;
}

// Access the global variable that is set every time the solenoid is written to to figure out the current solenoid value.
// Then, call a function to calculate the expected magnetic field strength due to the solenoid.
// Then, subtract out the solenoid's magnetic field to get just the magnetic field from the magnet.
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

// Apply either just the minmax filter, or both minmax and a moving average filter.
// This function takes an argument to determine whether the moving average filter should be applied.
int get_filtered_analog_reading(bool apply_moving_average) {
    int raw_val = get_sensor_value_with_solenoid_subtracted();
    int minmax_val = minmax_filter(raw_val);
    int filtered_val = 0;
    // add the option to skip the moving average filter step
    if (apply_moving_average) {
        filtered_val = moving_average_filter(minmax_val);
    } else {
        filtered_val = minmax_val;
    }
    if (PRINT_FILTERED_SENSOR_VALUE) {
        Serial.print(">analog value (1-1024) (filtered)");
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

// Apply the voltage-to-mT conversion defined in the hall_mT() function above to transform the analog reading
// into a magnetic field strength in milliTeslas.
float get_filtered_hall_effect_mT(bool apply_moving_average) {
    float val = hall_mT(get_filtered_analog_reading(apply_moving_average));
    if (PRINT_FILTERED_MT) {
        Serial.print(">Hall mT (filtered)");
        if (apply_moving_average) {
            Serial.print(":");
        } else {
            Serial.print(" (no mov.avg.):");
        }
        Serial.print(String(millis()) + ":");
        Serial.println(val);
    }
    return val;
}

// Apply the mT-to-distance conversion defined in the mT_to_distance() function above
// to return the distance of the magnet from the solenoid.
float get_filtered_distance_cm(bool apply_moving_average) {
    float val = mT_to_distance(get_filtered_hall_effect_mT(apply_moving_average));
    if (PRINT_FILTERED_DISTANCE) {
        Serial.print(">Distance cm (filtered)");
        if (apply_moving_average) {
            Serial.print(":");
        } else {
            Serial.print(" (no mov.avg.):");
        }
        Serial.print(String(millis()) + ":");
        Serial.println(val);
    }
    return val;
}

// Below, we defined aliases for the same functions but with no arguments,
// such that when no arguments are supplied, the default is true: do apply the moving average filter.

// Defining multiple functions with the same name but with different parameters is a
// practice in C++ coding called "overloading".
int get_filtered_analog_reading() {
    return get_filtered_analog_reading(true);
}

float get_filtered_hall_effect_mT() {
    return get_filtered_hall_effect_mT(true);
}

float get_filtered_distance_cm() {
    return get_filtered_distance_cm(true);
}
