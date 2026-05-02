/*
* DASH 2025-2026 Firmware
* Version: 1.0.0
* 
*/

// Includes -----------------

// Core Libraries
#include <Arduino.h>
#include "CANTProtocol.h"

#include <Wire.h>

// Specific Libraries
#include "Adafruit_LEDBackpack.h"
#include <Adafruit_GFX.h>

#include "config.h"

#include <Bounce2.h>

// Variables ----------------

bool state = true;
bool prevState = true;

uint16_t segments[] = {ALPHANUM_SEG_A,ALPHANUM_SEG_B,ALPHANUM_SEG_C,ALPHANUM_SEG_D,ALPHANUM_SEG_E,ALPHANUM_SEG_F,ALPHANUM_SEG_G1,ALPHANUM_SEG_G2,ALPHANUM_SEG_H,ALPHANUM_SEG_J,ALPHANUM_SEG_K,ALPHANUM_SEG_L,ALPHANUM_SEG_M,ALPHANUM_SEG_N};
char segName[] = {'A','B','C','D','E','F','G','g','H','J','K','L','M','N'};
// Instantiations -----------


Adafruit_AlphaNum4 upper = Adafruit_AlphaNum4();
Adafruit_AlphaNum4 lower = Adafruit_AlphaNum4();

Bounce button = Bounce();

// Functions ----------------

// SETUP --------------------

void setup(){

    pinMode(BUTTON_LOWER, INPUT_PULLUP);
    button.attach(BUTTON_LOWER);
    button.interval(5);

    Serial.begin(9600);

    // Alpha Setup
    upper.begin(I2C_ADDR_UPPER);
    lower.begin(I2C_ADDR_LOWER);

    upper.writeDigitAscii(0, 'U');
    upper.writeDigitAscii(1, 'o');
    upper.writeDigitAscii(2, 'F');
    upper.writeDigitAscii(3, 'A');
    upper.writeDisplay();


    delay(2000);
}

// MAIN ---------------------

void loop(){

    for(int index = 0; index < 14; index++){

        button.update();

        upper.clear();
        lower.clear();

        for(int digit = 0; digit < 4; digit++){
            upper.writeDigitRaw(digit,segments[index]);
            lower.writeDigitRaw(digit,segments[index]);
        }
        
        upper.writeDisplay();
        lower.writeDisplay();

        Serial.println(segName[index]);

        while(!button.fell()){
            button.update();
        }

        
    }

    
}