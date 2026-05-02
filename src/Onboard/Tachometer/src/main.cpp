#include <Arduino.h>
#include "CANTProtocol.h"

// 10 and 2 refer to D10 and D2 for the CS and the interrupt pin respectively
CANTProtocol CAN(10, 2, 0x4);

// Define the analog pin and constant
const int analogPin = A0;  // Analog input pin connected to the sensor
const float voltageReference = 5.0;  // Reference voltage for the ADC
const float k = 1000.0;  // Calibration constant for RPM calculation

float getRPM();

void builder(unsigned char dataLength, byte* incomingData, unsigned long callbackID) {
    float rpm = getRPM();
    CAN.sendRequestResponse(rpm, callbackID);
}   


void setup() {
    Serial.begin(115200);  // Initialize serial communication
    pinMode(analogPin, INPUT);  // Set the analog pin as an input
    

    CAN.registerRequest(0x00, builder);
    while(!CAN.begin()){
        // CAN.end();
        delay(100);
    }

    Serial.println("Init OK!");
}

void loop() {
    CAN.execute();
}

float getRPM(){
    // Read the analog value (0 to 1023)
    int analogValue = analogRead(analogPin);
    
    // Convert the analog value to a voltage (0.0V to 5.0V)
    float voltage = (analogValue / 1023.0) * voltageReference;
    
    // Calculate the engine speed using the equation:
    // line of best fit for rpm: rpm = = -70.424x2 + 1279.5x - 210.92

    float rpm = -16.046 * (voltage * voltage);  // Adjust the equation as needed for your application
    rpm += 1219.1 * voltage;
    rpm += -6.8501;

    // Output the result to the Serial Monitor
    // Serial.print("Voltage: ");
    // Serial.print(voltage, 2);  // Print voltage with 2 decimal places
    // Serial.print(" V, RPM: ");
    // Serial.println(rpm, 2);  // Print RPM with 2 decimal places
    return rpm;
}