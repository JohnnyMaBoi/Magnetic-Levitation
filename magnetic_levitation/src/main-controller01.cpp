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

int between(int lower, int upper, int val);

float hall_mT(int hall_analog) {
    // % solve for magnetic flux in units of mT
    // voltages are all in units of mV
    float hall_vout = (hall_analog / 1024.0) * 5000;
    float sensor_flux = (hall_vout - hall_vq) / (temp_effect);
    return sensor_flux;
}

const unsigned int hall_mT_moving_average_size = 4;
float hall_mT_moving_average_array[hall_mT_moving_average_size];
float prev_moving_average_value = 0;
float new_moving_average_value = 0;
int controller_val;

float moving_average_filter(float new_val, float* moving_average_array, unsigned int moving_average_len) {
    float moving_average = new_val;
    for (unsigned int i = 0; i < moving_average_len; i++) {
        moving_average += moving_average_array[moving_average_len - i];
        moving_average_array[moving_average_len - i - 1] = moving_average_array[moving_average_len - i - 2];
    }
    moving_average_array[0] = new_val;
    return moving_average / moving_average_len;
}

void setup() {
    Serial.begin(115200);

    // initialize moving average array to all zeros
    for (unsigned int i = 0; i < hall_mT_moving_average_size; i++) {
        hall_mT_moving_average_array[i] = 0;
    }

    Serial.println("");
    Serial.println("Sensor Reading in mT and raw analog");

    pinMode(MOTOR_IN1, OUTPUT);
    pinMode(MOTOR_IN2, OUTPUT);
    pinMode(SENSOR_PIN, INPUT);
    analogWrite(MOTOR_IN1, LOW);
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
