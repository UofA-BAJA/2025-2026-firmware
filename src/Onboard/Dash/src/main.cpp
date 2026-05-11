/*
* DASH 2025-2026 Firmware
* Version: 2.0.0
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
float carTime = 0;   // Car time         (data ID 3)
float distance = 0;  // Distance travelled (data ID 4)


// Other helpers
int displayIndex = 0;
int counter = 0;

// Instantiations -----------

CANTProtocol CAN(SPI_CS_CAN, SPI_INT_CAN, 9);

Adafruit_AlphaNum4 upper = Adafruit_AlphaNum4();
Adafruit_AlphaNum4 lower = Adafruit_AlphaNum4();

dog_7565R lcd;

Servo tachometer;
Servo speedometer;

Bounce button = Bounce();

// Functions ----------------

// Alpha Display Helpers

void writeText(Adafruit_AlphaNum4 &display, String text){

    display.clear();
    for (int i = 0; i < 4; i++) {
        display.writeDigitAscii(i, uint8_t(text.charAt(i)));
    }
    display.writeDisplay();

}

void updateDisplay(int index){
    switch(index){

        // RPM
        case 0:
            writeText(upper," RPM");
            writeText(lower, String(int(rpm)));
            break;

        // Speed
        case 1:
            writeText(upper,"VELO");
            writeText(lower, String(int(speed)));
            break;

        // CVT Temp
        case 2:
            writeText(upper,"TEMP");
            writeText(lower, String(int(cvtTemp)));
            break;

        // Car Time
        case 3:
            writeText(upper,"TIME");
            writeText(lower, String(int(carTime)));
            break;

        // Distance
        case 4:
            writeText(upper, "DIST");
            writeText(lower, String(int(distance)));
            break;

        // We don't talk about it
        default:
            break;
    }
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

// SETUP --------------------

void setup(){

    // Debug
    //Serial.begin(9600);

    // Alpha Setup
    upper.begin(I2C_ADDR_UPPER);
    lower.begin(I2C_ADDR_LOWER);

    writeText(upper, "UofA");
    writeText(lower, "Baja");
    delay(2000);

    // Allow allocation of all timers
	ESP32PWM::allocateTimer(0);
	ESP32PWM::allocateTimer(1);
	ESP32PWM::allocateTimer(2);
	ESP32PWM::allocateTimer(3);

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

    pinMode(BUTTON_LOWER, INPUT_PULLUP);
    button.attach(BUTTON_LOWER);
    button.interval(5);

    // CAN Setup
	pinMode(SPI_CS_CAN,OUTPUT);
	pinMode(SPI_INT_CAN,INPUT);
    SPI.begin(SPI_SCK, SPI_MISO, SPI_MOSI);

    CAN.registerCommand(0, onEngineRPM);
    CAN.registerCommand(1, onCarSpeed);
    CAN.registerCommand(2, onCVTTemp);
    CAN.registerCommand(3, onCarTime);
    CAN.registerCommand(4, onDistance);

    writeText(upper, "CAN ");
    writeText(lower, "INIT");

    while (!CAN.CAN.begin(MCP_STDEXT, CAN_500KBPS, MCP_16MHZ)) {
        delay(2000);
        //Serial.println("trying to connect...");
    }

    // CANTProtocol hardcodes MCP_8MHZ but the Adafruit CAN BFF has a 16 MHz crystal.
    // Re-init the MCP2515 with the correct clock; init_Mask/Filt handle config mode internally.
    CAN.CAN.init_Mask(0, 1, 0x1F000000);
    CAN.CAN.init_Filt(0, 1, 9UL << 24);
    CAN.CAN.init_Filt(1, 1, 9UL << 24);
    CAN.CAN.init_Mask(1, 1, 0x1F000000);
    CAN.CAN.init_Filt(2, 1, 9UL << 24);
    CAN.CAN.init_Filt(3, 1, 9UL << 24);
    CAN.CAN.init_Filt(4, 1, 9UL << 24);
    CAN.CAN.init_Filt(5, 1, 9UL << 24);

    

    writeText(lower, "DONE");
    delay(1000);

    lower.clear();
    upper.clear();

    updateDisplay(displayIndex);
}

// MAIN ---------------------

void loop(){
    CAN.execute();
    button.update();

    if(button.fell()){
        displayIndex = (displayIndex + 1) % 5;
        updateDisplay(displayIndex);
    }

}