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

const unsigned int hall_mT_moving_average_size = 25;
float hall_mT_moving_average_array[hall_mT_moving_average_size];

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
    analogWrite(MOTOR_IN2, 200);
    startMillis = millis();  // initial start time
}

void loop() {
    hall = analogRead(SENSOR_PIN);
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

    Serial.print(">hall_mT_filtered:");
    Serial.print(String(currentMillis) + ":");
    Serial.println(moving_average_filter(hall, hall_mT_moving_average_array, hall_mT_moving_average_size));
}
