#include <Arduino.h>
#include "CANTProtocol.h"
// #include <mcp_can.h>

CANTProtocol CAN(10,2,3);

const int SPI_CS_PIN = 10;

typedef struct speed{
    float left;
    float right;
    float rear;
} speed;

// // Define the analog pin and constant
const int analogPin0 = A0;  // Analog input pin connected to the sensor
const int analogPin1 = A1; 
const int analogPin2 = A2; 
const float voltageReference = 5.0;  // Reference voltage for the ADC

float getRPM0();
float getRPM1();
float getRPM2();

void builder(unsigned char dataLength, byte*incomingData, unsigned long callbackID){
    
    speed thisSpeed;
    thisSpeed.left = getRPM0();
    thisSpeed.right = getRPM1();
    thisSpeed.rear = getRPM2();
    
    CAN.sendRequestResponse(thisSpeed, callbackID);
}





void setup() {
    
    Serial.begin(115200);  // Initialize serial communication
    pinMode(analogPin0, INPUT);  // Set the analog pin as an input
    pinMode(analogPin1, INPUT);  // Set the analog pin as an input
    pinMode(analogPin2, INPUT);  // Set the analog pin as an input


    CAN.registerRequest(0,builder);
    CAN.begin();
    delay(500);

    Serial.println("Init OK!");

}


void loop() {
    CAN.execute();

    

}

float getRPM0() {
    int analogValue = analogRead(analogPin0);
    float voltage = (analogValue / 1023.0) * voltageReference;
    float rpm = 134 * voltage;  // Adjust the equation as needed for your application
    return rpm;
}

float getRPM1() {
  int analogValue = analogRead(analogPin1);
  float voltage = (analogValue / 1023.0) * voltageReference;
  float rpm = 134 * voltage;  // Adjust the equation as needed for your application
  return rpm;
}

float getRPM2() {
  int analogValue = analogRead(analogPin2);
  float voltage = (analogValue / 1023.0) * voltageReference;
  float rpm = 134 * voltage;  // Adjust the equation as needed for your application
  return rpm;
}