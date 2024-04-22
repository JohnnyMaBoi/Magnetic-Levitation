#include <Arduino.h>

// Live plotting of hall in teslas
// No activation of solenoid
// use to sanity check with only hall reading

// Plotting package: TELEPLOT, install with ctrl+p {ext install alexnesnes.teleplot}
// https://marketplace.visualstudio.com/items?itemName=alexnesnes.teleplot

#define MOTOR_IN1 9
#define MOTOR_IN2 10
#define SENSOR_PIN A0

// HALL CONVERSION AND GLOBAL VAR
// %terms from datasheet page 10, sens from table
// float Vq = 3.3/2;
// float sensitivity_term = 7.5; // changes by device & voltage
// float temp_effect = sensitivity_term*(1+(22-25)*.0012); // assuming 22 deg celsius
float temp_effect = 7.5 * (1 + (22 - 25) * .0012);  // assuming 22 deg celsius
int hall_vcc = 5000;                                // running voltage of hall, 5v in mV
float hall_vq = hall_vcc / 2;
int hall;  // analog output variable

// timing values
unsigned long startMillis;  // some global variables available anywhere in the program
unsigned long currentMillis;

float hall_mT(int hall_analog) {
    // % solve for magnetic flux in units of mT
    // voltages are all in units of mV
    float hall_vout = (hall_analog / 1024.0) * 5000;
    float sensor_flux = (hall_vout - hall_vq) / (temp_effect);
    return sensor_flux;
}

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
    // Serial.print(">max_val:");
    // Serial.print(String(millis()) + ":");
    // Serial.println(max_val);
    // Serial.print(">min_val:");
    // Serial.print(String(millis()) + ":");
    // Serial.println(min_val);
    moving_minmax_array[0] = new_val;
    if ((new_val > max_val_overall) || (new_val < min_val_overall)) {
        return (max_val + min_val) / 2;
    } else {
        return prev_val;
    }
}

void setup() {
    Serial.begin(115200);

    // initialize moving average array to all zeros
    for (unsigned int i = 0; i < moving_average_len; i++) {
        moving_average_array[i] = 0;
    }

    Serial.println("");
    Serial.println("Sensor Reading in mT and raw analog");

    pinMode(MOTOR_IN1, OUTPUT);
    pinMode(MOTOR_IN2, OUTPUT);
    pinMode(SENSOR_PIN, INPUT);
    analogWrite(MOTOR_IN1, LOW);
    analogWrite(MOTOR_IN2, 200);
    startMillis = millis();  // initial start time
}

int prev_minmax_hall = 0;
int minmax_hall = 0;
int moving_avg_hall;

void loop() {
    hall = analogRead(SENSOR_PIN);
    minmax_hall = minmax_filter(hall, prev_minmax_hall);
    prev_minmax_hall = minmax_hall;
    moving_avg_hall = moving_average_filter(minmax_hall);

    Serial.println(String(hall));
    // read sensor analog value
    currentMillis = millis();
    // plot processed and unprocessed result
    Serial.print(">hall_analog:");
    Serial.print(String(currentMillis) + ":");
    Serial.println(hall);

    Serial.print(">hall_mT:");
    Serial.print(String(currentMillis) + ":");
    Serial.println(hall_mT(hall));

    Serial.print(">hall_minmax_filtered:");
    Serial.print(String(currentMillis) + ":");
    Serial.println(minmax_hall);

    Serial.print(">hall_average_filtered:");
    Serial.print(String(currentMillis) + ":");
    Serial.println(moving_avg_hall);
}
