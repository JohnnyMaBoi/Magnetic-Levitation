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
unsigned long sample_time = 150;      // time for each sample in ms
unsigned long off_time = 500;
int samples_per_interval = 20;




void setup() {
    Serial.begin(115200);

    Serial.println("");
    Serial.println("DRV8871 test");
    Serial.println("");
    Serial.println("Analog Write Value, Sensor Reading");
    setup_solenoid();
    // turn_all_prints_on();
    turn_all_prints_off();
    // PRINT_FILTERED_DISTANCE = true;
    // PRINT_FILTERED_MT = true;
    // PRINT_RAW_SENSOR_VALUE = true;
    // PRINT_FILTERED_MT = true; 
    // PRINT_FILTERED_SENSOR_VALUE = true;
}



void loop() {
    // ramp up backward
    digitalWrite(MOTOR_IN2, LOW);
    write_solenoid(150);
    startMillis = millis();
    currentMillis = millis();
    while((currentMillis - startMillis)<off_time){
        currentMillis = millis();
        get_filtered_analog_reading(false);
        Serial.println(">raw val:"+String(millis())+":"+raw_val);
        Serial.println(">filtered val:"+String(millis())+":"+filtered_val);
        // Serial.println(i / samples_per_interval);
    }

    write_solenoid(255);
    startMillis = millis();
    currentMillis = millis();
    while((currentMillis - startMillis)<sample_time){
        currentMillis = millis();
        get_filtered_analog_reading(false);
        Serial.println(">raw val:"+String(millis())+":"+raw_val);
        Serial.println(">filtered val:"+String(millis())+":"+filtered_solenoid);

        // Serial.print(">Solenoid value (0-255):");
        // Serial.print(String(loop_timer) + ":");
        // Serial.println(i / samples_per_interval);
    }
}
