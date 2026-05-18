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

#include <Bounce2.h>

// Variables ----------------


// Other helpers
int lowerPos = 0;

// Instantiations -----------

Adafruit_AlphaNum4 upper = Adafruit_AlphaNum4();
Adafruit_AlphaNum4 lower = Adafruit_AlphaNum4();

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


// SETUP --------------------

void setup(){

    // Debug
    Serial.begin(9600); 
    Serial.println("Entering setup...");

    // Alpha Setup
    Wire.begin(I2C_SCL,I2C_SDA);
    upper.begin(I2C_ADDR_UPPER);
    lower.begin(I2C_ADDR_LOWER);

    lower.clear();
    upper.clear();

    //SPI.begin();

    Serial.println("I2C init done...");

    writeText(upper, "UofA");
    writeText(lower, "SAE.");
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

    Serial.println("servoes set up...");

    pinMode(BUTTON_LOWER, INPUT_PULLUP);
    button.attach(BUTTON_LOWER);
    button.interval(5);

    Serial.println("button set up...");

    Serial.println("CAN commands registered...");

    lower.clear();
    upper.clear();
}
// MAIN ---------------------

void loop(){

    if(button.fell()){
        Serial.println("Updating stuff.");
        lower.clear();
        writeText(lower,String(lowerPos));
        speedometer.write(lowerPos);
        lowerPos++;
        Serial.println("Done.");
    }
}