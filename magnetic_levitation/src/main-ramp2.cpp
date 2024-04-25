#include <Arduino.h>

#include "constants.h"
#include "sensor_funcs.h"
#include "solenoid_funcs.h"

// Basic sketch for trying out the Adafruit DRV8871 Breakout
// at 13 Ohms
// -3.83V maximum (repeated twice)
// + 3.83V maximum (repeated twice)
// super non-linear though. It's much slower near small signals and changes much more rapidly above ~1V

// timing values
unsigned long startMillis;  // some global variables available anywhere in the program
unsigned long currentMillis;
int samples_per_interval = 10;

// output variables
int filtered_analog_reading;  //combination average and min-max filter output
int corrected_analog_reading; // remove solenoid impact from hall analog
float distance_cm;
float mT_out;


void print_all_values(int i) {
    // take readings
    filtered_analog_reading = get_filtered_analog_reading();
    corrected_analog_reading = filtered_analog_reading - solenoid_hall_correction_analog;
    mT_out = hall_mT(corrected_analog_reading);
    distance_cm = mT_to_distance(mT_out);
    currentMillis = millis()-startMillis;

    currentMillis = millis();
    Serial.print(">solenoid_value:");
    Serial.print(String(currentMillis) + ":");
    Serial.println(i / samples_per_interval);

    Serial.print(">raw_hall_analog:");
    Serial.print(String(currentMillis) + ":");
    Serial.println(analogRead(SENSOR_PIN) - 509);

    Serial.print(">filtered_hall_analog:");
    Serial.print(String(currentMillis) + ":");
    // Serial.println(get_filtered_analog_reading() - solenoid_hall_correction_analog);
    Serial.println(corrected_analog_reading);


    Serial.print(">Distance:");
    Serial.print(String(currentMillis) + ":");
    Serial.println(distance_cm);

}

void setup() {
    Serial.begin(115200);

    Serial.println("");
    Serial.println("DRV8871 test");
    Serial.println("");
    Serial.println("Analog Write Value, Sensor Reading");
    setup_solenoid();
}

void loop() {
    // ramp up backward
    digitalWrite(MOTOR_IN2, LOW);
    for (int i = 0; i < 255 * samples_per_interval; i++) {
        write_solenoid(i / samples_per_interval);
        print_all_values(i);
    }

    // ramp down backward
    for (int i = 255 * samples_per_interval; i >= 0; i--) {
        write_solenoid(i / samples_per_interval);
        print_all_values(i);
    }
}
