/*
* DASH REV2 2025-2026 Firmware
* Version: 1.0.0
* 
*/

// Includes -----------------

// Core Libraries
#include <Arduino.h>
#include "CANTProtocol.h"
#include <string.h>

#include <Wire.h>

#include "config.h"

// Specific Libraries
#include <ESP32Servo.h>

#include "Adafruit_LEDBackpack.h"
#include <Adafruit_GFX.h>

#include "dog_7565R.h"

#include <Bounce2.h>

// Variables ----------------

// CAR DATA

float rpm = 0;      // Engine RPM      (data ID 0)
float speed = 0;     // Car speed        (data ID 1)
float cvtTemp = 0;   // CVT temperature  (data ID 2)
int carTime = 0;   // Car time         (data ID 3)
float distance = 0;  // Distance travelled (data ID 4)


// Other helpers
int displayIndex = 0;
int counter = 0;

// Instantiations -----------

CANTProtocol CAN(SPI_CS_CAN, SPI_INT_CAN, CAN_ADDR);

Adafruit_AlphaNum4 upper = Adafruit_AlphaNum4();
Adafruit_AlphaNum4 lower = Adafruit_AlphaNum4();

dog_7565R lcd;

Servo tachometer;
Servo speedometer;

Bounce button = Bounce();

// Functions ----------------

void writeText(Adafruit_AlphaNum4 &display, String text){

    display.clear();
    for (int i = 0; i < 4; i++) {
        display.writeDigitAscii(i, uint8_t(text.charAt(i)));
    }
    display.writeDisplay();

}

// CAN Command Handlers -----

void onEngineRPM(unsigned char dataLength, byte* incomingData, unsigned long callbackID) {
    memcpy(&rpm, incomingData, sizeof(float));

    tachometer.write(int(map(rpm,0,4000,SERVO_TACH_MIN,SERVO_TACH_MAX)));
    
    if(displayIndex == 0) updateDisplay(0);
}

void onCarSpeed(unsigned char dataLength, byte* incomingData, unsigned long callbackID) {
    memcpy(&speed, incomingData, sizeof(float));

    speedometer.write(int(map(speed,0,40,SERVO_SPEED_MIN,SERVO_SPEED_MAX)));
    
    if(displayIndex == 1) updateDisplay(1);
}

void onCVTTemp(unsigned char dataLength, byte* incomingData, unsigned long callbackID) {
    memcpy(&cvtTemp, incomingData, sizeof(float));
    if(displayIndex == 2) updateDisplay(2);
}

void onCarTime(unsigned char dataLength, byte* incomingData, unsigned long callbackID) {
    memcpy(&carTime, incomingData, sizeof(float));
    if(displayIndex == 3) updateDisplay(3);
}

void onDistance(unsigned char dataLength, byte* incomingData, unsigned long callbackID) {
    memcpy(&distance, incomingData, sizeof(float));
    if(displayIndex == 4) updateDisplay(4);
}

void updateDisplay(int index){
    switch(index){

        // RPM
        case 0:
            writeText(upper," RPM");
            writeText(lower, String(round(rpm)));
            break;

        // Speed
        case 1:
            writeText(upper,"SPEED");
            writeText(lower, String(round(speed)));
            break;

        // CVT Temp
        case 2:
            writeText(upper,"TEMP");
            writeText(lower, String(round(cvtTemp)));
            break;

        // Car Time
        case 3:
            writeText(upper,"TIME");
            writeText(lower, String(round(carTime)));
            break;

        // Distance
        case 4:
            writeText(upper, "DIST");
            writeText(lower, String(round(distance)));
            break;

        // We don't talk about it
        default:
            break;
    }
}
// SETUP --------------------

void setup(){

    // ALPHANUMERIC SETUP ===

    upper.begin(I2C_ADDR_UPPER);
    lower.begin(I2C_ADDR_LOWER);

    writeText(upper, "UofA");
    writeText(lower, "Baja");
    delay(2000);

    // SERVO SETUP ===

    // Servo Setup
    tachometer.setPeriodHertz(50);
    tachometer.attach(SERVO_TACH, 600, 2400);

    speedometer.setPeriodHertz(50);
    speedometer.attach(SERVO_SPEED, 600, 2400);

    tachometer.write(0);
    speedometer.write(0);
    delay(500);
    tachometer.write(180);
    speedometer.write(180);
    delay(500);
    tachometer.write(0);
    speedometer.write(0);
    delay(500);

    // LCD SETUP ===

    // LCD Setup (not enough pins)
    //lcd.initialize(, 35, 36, A0_PIN, RESET_PIN, DOGM128);
    //lcd.contrast(20);

    // CAN SETUP ===

	pinMode(SPI_CS_CAN,OUTPUT);
	pinMode(SPI_INT_CAN,OUTPUT);

    CAN.registerCommand(0, onEngineRPM);
    CAN.registerCommand(1, onCarSpeed);
    CAN.registerCommand(2, onCVTTemp);
    CAN.registerCommand(3, onCarTime);
    CAN.registerCommand(4, onDistance);

    writeText(upper, "CAN ");
    writeText(lower, "INIT");

    while (!CAN.begin()) {
        delay(2000);
    }
    delay(1000);

    writeText(lower, "READY");

    lower.clear();
    upper.clear();
}


// MAIN ---------------------

void loop(){
    
    CAN.execute();
    button.update();

    if(!button.fell()){
        displayIndex = (displayIndex + 1) % 5;
        updateDisplay(displayIndex);
    }
}