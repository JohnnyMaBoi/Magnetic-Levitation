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
float temp_effect = 7.5*(1+(22-25)*.0012); // assuming 22 deg celsius 
int hall_vcc = 3300; // running voltage of hall, 3.3 v in mV
float hall_vq = hall_vcc/2;
int hall; // analog output variable 

// timing values
unsigned long startMillis;  //some global variables available anywhere in the program
unsigned long currentMillis;

float hall_mT(int hall_analog){
    // % solve for magnetic flux in units of mT
    // voltages are all in units of mV
    int hall_vout = (hall_analog/1024)*5000; 
    float sensor_flux = (hall_vout - hall_vq)/(temp_effect);
    return sensor_flux;
}

void setup() {
    Serial.begin(115200);

    Serial.println("");
    Serial.println("Sensor Reading in mT and raw analog");

    pinMode(MOTOR_IN1, OUTPUT);
    pinMode(MOTOR_IN2, OUTPUT);
    pinMode(SENSOR_PIN, INPUT);
    startMillis = millis();  //initial start time
}


void loop() {
    
    Serial.println(String(analogRead(SENSOR_PIN)));
    // read sensor analog value
     hall = analogRead(SENSOR_PIN);
    currentMillis = millis();
    // plot processed and unprocessed result
    Serial.print(">hall_mT:");
    Serial.print(String(currentMillis));
    Serial.println(hall_mT(hall));

    Serial.print(">hall_analog:");
    Serial.print(String(currentMillis));
    Serial.println(hall);

}
