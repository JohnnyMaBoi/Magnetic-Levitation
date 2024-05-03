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
unsigned long currentMillis;    // monitoring against loop beginning
unsigned long actionMillis;       // start of sub-action (loop, gap between tests) time

// sampling variables
int test_completed = 0;
int seconds_per_sample = 5;
int soln_low = 20;
int sol_med = 125;
int sol_high = 245;
int sol_cmd;    // what analog is commanded to slenoid right now?

// output variables
int filtered_analog_reading;  //combination average and min-max filter output
float distance_cm;
float mT_out;



void setup() {
    Serial.begin(115200);

    Serial.println("Guided Testing script for 5 second samples with and without solenoid/magnet");
    Serial.println("");
    Serial.println("");
    pinMode(MOTOR_IN1, OUTPUT);
    pinMode(MOTOR_IN2, OUTPUT);
    digitalWrite(MOTOR_IN1, LOW);

}

void print_message(char* message){
    Serial.println("-------------------------");
    Serial.println("");
    Serial.println(message);
    Serial.println("");
    Serial.println("-------------------------");
}

// void delay_test(unsigned long* current_millis, unsigned long* actionMillis){
//     while 
// }

void loop() {
    if (test_completed == 0){
        // ramp up backward (attract magnet)
        digitalWrite(MOTOR_IN2, LOW);
        print_message("Move Magnet Far Away: you have 5 seconds");

        // delay between tests
        // delay_test(&actionMillis, &currentMillis);
        actionMillis = millis();    
        currentMillis = millis();
        while((currentMillis-actionMillis)<(seconds_per_sample*1000)){
            currentMillis = millis();
            delay(100);
        }

        // test setup
        sol_cmd = 0;    //test with no solenoid
        analogWrite(MOTOR_IN2, sol_cmd);
        print_message("Test Starting: No Magnet, No Solenoid");
        actionMillis = millis();    
        currentMillis = millis();
        // sample loop
        while(currentMillis-actionMillis<(seconds_per_sample*1000)){
            filtered_analog_reading = get_filtered_analog_reading();    
            mT_out = hall_mT(moving_avg_hall);                   
            distance_cm = mT_to_distance(mT_out);
            currentMillis = millis();

            Serial.print(">hall_mT:");
            Serial.print(String(currentMillis) + ":");
            Serial.println(mT_out);

            Serial.print(">solenoid_analog_commanded:");
            Serial.print(String(currentMillis) + ":");
            Serial.println(sol_cmd);

            Serial.print(">magnet_distance:");
            Serial.print(String(currentMillis) + ":");
            Serial.println(distance_cm);

        }
        
        test_completed = 1;
    }


}
