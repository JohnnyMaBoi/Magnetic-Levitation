#include <Arduino.h>

#include "constants.h"
#include "sensor_funcs.h"
#include "solenoid_funcs.h"

// timing values
unsigned long startMillis;  // some global variables available anywhere in the program
unsigned long currentMillis;

int between(int lower, int upper, int val);

const unsigned int hall_mT_moving_average_size = 4;
float hall_mT_moving_average_array[hall_mT_moving_average_size];
float prev_moving_average_value = 0;
float new_moving_average_value = 0;
int controller_val;

void setup() {
    Serial.begin(115200);
    setup_moving_average_array();
    setup_solenoid();
    startMillis = millis();  // initial start time
}

float Kp = 5;
float Kd = Kd / 30;
unsigned long last_controller_millis = 0;
float prev_distance_mm = 0;
int controller(float distance_mm) {
    unsigned long current_millis = millis();
    unsigned long dt_milliseconds = current_millis - last_controller_millis;
    last_controller_millis = current_millis;
    float dhalldt_mm_per_s = float(distance_mm - prev_distance_mm) / float(dt_milliseconds * MILLISECONDS_TO_SECONDS);
    return between(50, 250, int(Kd * dhalldt_mm_per_s) + int(Kp * distance_mm));
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

void loop() {
    currentMillis = millis();
    float distance_mm = get_filtered_distance_cm() * 10;
    controller_val = controller(distance_mm);
    write_solenoid(controller_val);

    Serial.print(">hall_mT_filtered:");
    Serial.print(String(currentMillis) + ":");
    Serial.println(new_moving_average_value);

    Serial.print(">controller val:");
    Serial.print(String(currentMillis) + ":");
    Serial.println(controller_val);

    analogWrite(MOTOR_IN2, controller_val);
}
