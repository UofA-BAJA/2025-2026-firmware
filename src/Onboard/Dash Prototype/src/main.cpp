/*
* Display test for Baja Dashboard
*
*/

// Core libraries
#include <Arduino.h>
#include <UMS3.h>
#include <string>

// Configuration vars
#include "config.h"

// LCD-specific libraries
#include "bajaLCD.h"
#include "fonts.h"
#include "images.h"

// Alphanumeric LED libraries
#include <Wire.h>
#include <SparkFun_Alphanumeric_Display.h>

// Create LCD object
bajaLCD display;

// Create Alphanumeric object
HT16K33 upper;
HT16K33 lower;

// Create board object
UMS3 board;
int statusColor = 0;



void TestScreen(const char *upper, const char *lower) {
  display.clear();

  display.print(1,font_8x16, upper,ALIGN_RIGHT);

  display.box(5,3,123,4,0xFF);
  
  display.print(5,font_8x16, lower, ALIGN_CENTER);
}


void setup() {

  // Board and Status LED setup
  board.begin();
  board.setPixelPower(true);
  board.setPixelBrightness(64);

  // Set to initialization status
  board.setPixelColor(0,0,255);
  
  // Initialize display with hardware SPI
  // For hardware SPI, use MOSI for both parameters
  display.init(LCD_CS_PIN, MOSI, MOSI, LCD_A0_PIN, LCD_RESET_PIN);

  // Set contrast
  display.setContrast(20);
  
  display.clear();
  display.image(0,0,baja_logo_dark);
  delay(3000);

  Wire.begin();
  
  upper.begin(ALPHA_UPPER_ADDR);
  lower.begin(ALPHA_LOWER_ADDR);

  board.setPixelColor(255,128,0);

  int alphaCounter = 0;
  while((upper.begin() == false || lower.begin() == false) && alphaCounter < 100) {
    display.clear();
    display.print(0,font_8x16, "Waiting on alphas");
    alphaCounter++;
    delay(10);
  }

  upper.setBrightness(15);
  lower.setBrightness(15);

  if(DEBUG) Serial.println("Brightness set!");

  upper.print("UPPR");
  lower.print("LOWR");



  TestScreen("UPPER", "LOWER");

  //Set board status to OK
  board.setPixelColor(0,255,0);
}

void loop() {

  delay(30);
  statusColor++;
  upper.print(statusColor);
  lower.print(statusColor);
  for(byte i = 0; i < 8; i++){
    upper.shiftRight();
    lower.shiftLeft();
    delay(100);
  }
}