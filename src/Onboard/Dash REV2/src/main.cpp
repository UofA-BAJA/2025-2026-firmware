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

//#include <SPI.h>
#include <Wire.h>

#include "config.h"

// Specific Libraries
#include <ESP32Servo.h>

#include "Adafruit_LEDBackpack.h"
#include <Adafruit_GFX.h>

//#include "dog_7565R.h"
//#include "fonts.h"
//#include "images.h"

#include <Bounce2.h>

// Variables ----------------

// CAR DATA

float rpm = 0;      // Engine RPM      (data ID 0)
float speed = 0;     // Car speed        (data ID 1)
float cvtTemp = 0;   // CVT temperature  (data ID 2)
float carTime = 0;   // Car time         (data ID 3)
float distance = 0;  // Distance travelled (data ID 4)


// Other helpers
int displayIndex = 2;
int counter = 0;

// Instantiations -----------

//CANTProtocol CAN(SPI_CS_CAN, SPI_INT_CAN, CAN_ADDR);

Adafruit_AlphaNum4 upper = Adafruit_AlphaNum4();
Adafruit_AlphaNum4 lower = Adafruit_AlphaNum4();
//dog_7565R lcd;
Servo tachometer;
Servo speedometer;

// Bounce button = Bounce();

// Functions ----------------

// Alpha Display Helpers

void writeText(Adafruit_AlphaNum4 &display, String text){

    display.clear();
    for (int i = 0; i < 4; i++) {
        display.writeDigitAscii(i, uint8_t(text.charAt(i)));
    }
    display.writeDisplay();

}

void updateAlphas(int index){
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


/*void displayMenu(String items[], int count, int selected) {
    lcd.clear();
    lcd.string(20, 0, font_8x8, "== MENU ==");
    
    for (int i = 0; i < count && i < 5; i++) {
      if (i == selected) {
        lcd.string(0, i + 2, font_6x8, ">");
        lcd.string(10, i + 2, font_6x8, items[i].c_str());
      } else {
        lcd.string(10, i + 2, font_6x8, items[i].c_str());
      }
    }
}*/


// CAN Command Handlers -----

void onEngineRPM(unsigned char dataLength, byte* incomingData, unsigned long callbackID) {
    memcpy(&rpm, incomingData, sizeof(float));

    tachometer.write(constrain(map(rpm,0,4000,SERVO_TACH_MIN,SERVO_TACH_MAX), SERVO_TACH_MIN, SERVO_TACH_MAX));
    
    // if(displayIndex == 0) updateAlphas(0);
}

void onCarSpeed(unsigned char dataLength, byte* incomingData, unsigned long callbackID) {
    memcpy(&speed, incomingData, sizeof(float));

    speedometer.write(constrain(map(speed,0,40,SERVO_SPEED_MIN,SERVO_SPEED_MAX), SERVO_SPEED_MAX, SERVO_SPEED_MIN));
    
    // if(displayIndex == 1) updateAlphas(1);
}

void onCVTTemp(unsigned char dataLength, byte* incomingData, unsigned long callbackID) {
    memcpy(&cvtTemp, incomingData, sizeof(float));
    // if(displayIndex == 2) updateAlphas(2);
}

void onCarTime(unsigned char dataLength, byte* incomingData, unsigned long callbackID) {
    memcpy(&carTime, incomingData, sizeof(float));
    // if(displayIndex == 3) updateAlphas(3);
}

void onDistance(unsigned char dataLength, byte* incomingData, unsigned long callbackID) {
    memcpy(&distance, incomingData, sizeof(float));
    // if(displayIndex == 4) updateAlphas(4);
}

// SETUP --------------------

void setup(){

    // Debug
    Serial.begin(9600); 
    Serial.println("Entering setup...");

    // Alpha Setup
    //Wire.begin(I2C_SDA,I2C_SCL);
    upper.begin(I2C_ADDR_UPPER);
    lower.begin(I2C_ADDR_LOWER);

    lower.clear();
    upper.clear();

    //SPI.begin();

    Serial.println("I2C init done...");

    Serial.println("Entering CAN init loop");
    // while (!CAN.begin()) {
        // delay(2000);
        // Serial.println("CAN init failed, retrying...");
    // }

    writeText(upper, "UofA");
    writeText(lower, "Baja");
    delay(2000);

    // LCD Setup
    // Initialize display with hardware SPI
    // For hardware SPI, use MOSI for both parameters
    /*Serial.println("entering LCD Setup...");
    lcd.initialize(SPI_CS_LCD, SPI_MOSI, SPI_SCK, LCD_A0, LCD_RST, DOGM128);
    lcd.contrast(27);
    lcd.view(0xC8);
    lcd.clear();

    lcd.picture(0,0,baja_logo_dark);
    Serial.println("LCD set up...");*/

    // Allow allocation of all timers
	//ESP32PWM::allocateTimer(1);
	//ESP32PWM::allocateTimer(2);
    //ESP32PWM::allocateTimer(0);
	//ESP32PWM::allocateTimer(3);

    //Servo Setup
    tachometer.setPeriodHertz(50);
    tachometer.attach(SERVO_TACH, 600, 2400);

    speedometer.setPeriodHertz(50);
    speedometer.attach(SERVO_SPEED, 600, 2400);

    tachometer.write(SERVO_TACH_MIN);
    speedometer.write(SERVO_SPEED_MIN);
    delay(1000);
    tachometer.write(SERVO_TACH_MAX);
    speedometer.write(SERVO_SPEED_MAX);
    delay(1000);
    tachometer.write(SERVO_TACH_MIN);
    speedometer.write(SERVO_SPEED_MIN);
    delay(1000);

    Serial.println("servoes set up...");

    //pinMode(BUTTON_LOWER, INPUT_PULLUP);
    // button.attach(BUTTON_LOWER);
    // button.interval(5);

    Serial.println("button set up...");

    // CAN Setup

    Serial.println("CAN commands registered...");

    writeText(upper, "CAN ");
    writeText(lower, "INIT");
    

    // CAN.registerCommand(0, onEngineRPM);
    // CAN.registerCommand(1, onCarSpeed);
    // CAN.registerCommand(2, onCVTTemp);
    // CAN.registerCommand(3, onCarTime);
    // CAN.registerCommand(4, onDistance);

    writeText(lower, "DONE");
    //delay(1000);

    //lower.clear();
    //upper.clear();

    //updateAlphas(displayIndex);
}
// MAIN ---------------------

void loop(){
    
    //CAN.execute();
    //button.update();

    // if(button.fell()){
        // Serial.println("Updating stuff.");
        // displayIndex = (displayIndex + 1) % 5;
        // updateAlphas(displayIndex);
        // Serial.println("Done.");
    // }
}