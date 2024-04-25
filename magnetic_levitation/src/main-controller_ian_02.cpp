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

int steady_state_output = 207;
float steady_state_hall_effect = 34;
float Kp = 5;
float Kd = Kd / 30;
int controller(float hall_mT, float prev_hall_mT) {
    float hall_difference = hall_mT - steady_state_hall_effect;
    float dhalldt = hall_mT - prev_hall_mT;
    return between(50, 250, steady_state_output - int(Kd * dhalldt) - int(Kp * hall_difference));
    // return between(50, 250, steady_state_output - int(Kp * hall_difference));
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
    hall = analogRead(SENSOR_PIN);
    Serial.println(String(hall));
    // read sensor analog value
    currentMillis = millis();
    // plot processed and unprocessed result
    prev_moving_average_value = new_moving_average_value;
    new_moving_average_value = moving_average_filter(hall_mT(hall), hall_mT_moving_average_array, hall_mT_moving_average_size);
    controller_val = controller(new_moving_average_value, prev_moving_average_value);

    Serial.print(">hall_mT_filtered:");
    Serial.print(String(currentMillis) + ":");
    Serial.println(new_moving_average_value);

    Serial.print(">controller val:");
    Serial.print(String(currentMillis) + ":");
    Serial.println(controller_val);

    analogWrite(MOTOR_IN2, controller_val);
}
