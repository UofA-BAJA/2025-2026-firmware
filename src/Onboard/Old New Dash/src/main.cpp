#include <Arduino.h>
#include <Wire.h>
#include <SparkFun_Alphanumeric_Display.h>
#include <ESP32Servo.h>
#include "CANTProtocol.h"
#include <Adafruit_LEDBackpack.h>
#include <Preferences.h>

//Methods
void writeDisplays(void *pvParameters);
void displayCVTTemp(HT16K33 disp);
void displayTime(HT16K33 disp);
void displayRPM(HT16K33 disp);
void displaySpeed(HT16K33 disp);
void displayDistance(HT16K33 disp);

// Constants
const int CAN_CS_PIN = 5;
const int CAN_INT_PIN = 27;
const unsigned long DASH_DEVICE_ID = 9;
const int DISPLAY1_BUTTON_PIN = 12;
const int DISPLAY2_BUTTON_PIN = 14;
enum displayOptions{
  CVT_TEMP = 0,
  TIME,
  RPM,
  SPEED,
  DISTANCE,
  END_ELEMENT
};
const String version = "V2-0-0";
const int RPM_MIN_ANGLE = 168;
const int RPM_MAX_ANGLE = 0;
const int SPEED_MIN_ANGLE = 5;
const int SPEED_MAX_ANGLE = 180;

// Devices
HT16K33 display; // 14 segment
HT16K33 display2;
CANTProtocol CAN(CAN_CS_PIN, CAN_INT_PIN, DASH_DEVICE_ID);
Servo speed;
Servo rpm;
Adafruit_LEDBackpack ledMatrix = Adafruit_LEDBackpack();

Preferences prefs;

// Variables
float lastSpeed = 0;
float lastRPM = 0;
float lastCVTTemp = 0;
float lastTimeSeconds = 0;
float lastDistance = 0;
uint16_t indicatorLightState = 0;

bool canRecentRX = true;
unsigned long lastCANRx = 0;

int display1CurrentDisplay = 0;
unsigned long lastDisp1Button = 0; //Last button press time
int display2CurrentDisplay = 1;
unsigned long lastDisp2Button = 0; //Last button press time
bool areBothHeld = false; //Hold for 2 seconds to reset 
unsigned long bothHeldStart = 0;

// CAN Command Callbacks
void onSpeed(unsigned char dataLength, byte* incomingData, unsigned long callbackID) {
  memcpy(&lastSpeed, incomingData, sizeof(float));
  canRecentRX = true;
  lastCANRx = millis();
}

void onRPM(unsigned char dataLength, byte* incomingData, unsigned long callbackID) {
  memcpy(&lastRPM, incomingData, sizeof(float));
  canRecentRX = true;
  lastCANRx = millis();
}

void onCVTTemp(unsigned char dataLength, byte* incomingData, unsigned long callbackID) {
  memcpy(&lastCVTTemp, incomingData, sizeof(float));
  canRecentRX = true;
  lastCANRx = millis();
}

void onTime(unsigned char dataLength, byte* incomingData, unsigned long callbackID) {
  memcpy(&lastTimeSeconds, incomingData, sizeof(float));
  canRecentRX = true;
  lastCANRx = millis();
}

void onIndicatorLights(unsigned char dataLength, byte* incomingData, unsigned long callbackID) {
  memcpy(&indicatorLightState, incomingData, sizeof(uint16_t));
  canRecentRX = true;
  lastCANRx = millis();
}

void onDistance(unsigned char dataLength, byte* incomingData, unsigned long callbackID) {
  memcpy(&lastDistance, incomingData, sizeof(float));
  canRecentRX = true;
  lastCANRx = millis();
}


void setup()
{
  // <insert that default comment that gets generated with all arduino projects>
  delay(3000);
  Serial.begin(115200);
  Serial.print("Dash Firmware ");
  Serial.println(version);

  // Join I2C
  Wire.begin();

  //---------------------------------------------------

  // initalize 14 segment and check for display acknowledge
  if (display.begin(0x70, 0x71) == false)
  {
    Serial.println("[WARNING] At least one 14 segment from row 1 did not acknowledge!");
  }
  if (display2.begin(0x72, 0x73) == false)
  {
    Serial.println("[WARNING] At least one 14 segment from row 2 did not acknowledge!");
  }
  Serial.println("Display setup complete.");

  //---------------------------------------------------

  // Initalize Servos
  speed.attach(33);
  rpm.attach(32);
  Serial.println("Servos initialized.");

  //---------------------------------------------------

  // Initialize LED Matrix
  if (ledMatrix.begin(0x74) == false)
  {
    Serial.println("[WARNING] LED Matrix Driver did not acknowledge!");
  }
  Serial.println("LED Matrix setup complete.");

  //---------------------------------------------------
  
  //Initialize pins for buttons
  pinMode(DISPLAY1_BUTTON_PIN, INPUT_PULLUP);
  pinMode(DISPLAY2_BUTTON_PIN, INPUT_PULLUP);

  //---------------------------------------------------

  //Light Test
  if(digitalRead(DISPLAY1_BUTTON_PIN) == LOW || digitalRead(DISPLAY2_BUTTON_PIN) == LOW){
    //While still holding the buttons...
    while(digitalRead(DISPLAY1_BUTTON_PIN) == LOW || digitalRead(DISPLAY2_BUTTON_PIN) == LOW){
      display.print("LIGHT");
      display2.print("TEST");
      delay(1000);
      display.clear();
      display2.clear();
      ledMatrix.displaybuffer[0] = 0b1111111111111111;
      ledMatrix.displaybuffer[1] = 0b1111111111111111;
      ledMatrix.writeDisplay();
      delay(2000);
      ledMatrix.clear();
      ledMatrix.writeDisplay();
      //Define an all-on character and print it out, then turn on decimal and colon
      display.defineChar('`', 0b1111111111111111);
      display2.defineChar('`', 0b1111111111111111);
      display.print("````````");
      display2.print("````````");
      display.colonOn();
      display2.colonOn();
      display.decimalOn();
      display2.decimalOn();
      delay(2000);
      display.clear();
      display2.clear();
      //Sweep the servos
      speed.write(SPEED_MAX_ANGLE);
      rpm.write(RPM_MAX_ANGLE);
      delay(1500);
      speed.write(SPEED_MIN_ANGLE);
      rpm.write(RPM_MIN_ANGLE);
      delay(1500);
      Serial.println("Light test complete.");
      //Will loop if a button is held
      display.print("AGAIN?");
      delay(2000);
    }
  }

  display.print("WELCOME");
  display2.print("TO BAJA");
  speed.write(SPEED_MIN_ANGLE);
  rpm.write(RPM_MIN_ANGLE);
  ledMatrix.displaybuffer[0] = 0b1010100000000000;
  ledMatrix.displaybuffer[1] = 0b1010100000000000;
  ledMatrix.writeDisplay();
  delay(1500);
 
  //---------------------------------------------------

  // Initialize CAN via CANTProtocol
  if (!CAN.begin())
  {
    Serial.println("[ERROR] CAN Init Failed");
    display.print("CAN FAIL");
    display2.print("CANTPROT");
    delay(5000);
    ledMatrix.displaybuffer[1] = 0;
    ledMatrix.displaybuffer[0] = 0;
    ledMatrix.writeDisplay();
    display.clear();
    display2.clear();
    speed.write(SPEED_MIN_ANGLE);
    rpm.write(RPM_MIN_ANGLE);
    delay(500);
    ESP.restart();
  }
  Serial.println("CAN Init OK!");

  // Register CANT command handlers (data IDs match CC sender)
  CAN.registerCommand(0x00, onRPM);
  CAN.registerCommand(0x01, onSpeed);
  CAN.registerCommand(0x02, onCVTTemp);
  CAN.registerCommand(0x03, onTime);
  CAN.registerCommand(0x04, onDistance);
  CAN.registerCommand(0x05, onIndicatorLights);

  Serial.println("CAN setup complete");

  //---------------------------------------------------

  //Load Display preferences
  prefs.begin("DisplayPrefs", false);
  if(!prefs.isKey("Display1") || !prefs.isKey("Display2")){
    //Put in "Factory Defaults" otherwise it will error (example: swapping ESP32s)
    prefs.putInt("Display1", display1CurrentDisplay);
    prefs.putInt("Display2", display2CurrentDisplay);
  }else{
    //Load in whatever's stored 
    display1CurrentDisplay = prefs.getInt("Display1");
    display2CurrentDisplay = prefs.getInt("Display2");
  }
  prefs.end();

  //---------------------------------------------------

  xTaskCreate(
    writeDisplays,
    "Write Displays",
    4096,
    NULL, 
    10,
    NULL
  );

  //Init OK!
  display2.clear();
  display.print("INIT OK");
  display2.print(version);
  Serial.println("Init Finished!");
  delay(1500);
  ledMatrix.displaybuffer[0] = 0;
  ledMatrix.displaybuffer[1] = 0;
  ledMatrix.writeDisplay();
}



void loop()
{

 CAN.execute();

  //If we haven't recieved a CAN command in 2 seconds, assume CAN is offline
  if(millis()-lastCANRx > 2000 && canRecentRX){
    canRecentRX = false;
  }
}



void writeDisplays(void *pvParameters){
  delay(1500);
  int num = 0; //cycle counter for arbitrary delay
  while(true){
    num++;

    bool tempCANRecentRX = canRecentRX;

    // If there has not been can recently, flash NO CAN
    if(num%8==0 && !tempCANRecentRX){
      ledMatrix.displaybuffer[0] = indicatorLightState | (1 << 8);
      ledMatrix.displaybuffer[1] = indicatorLightState | (1 << 8);
      ledMatrix.writeDisplay();
      if(num%16 == 0){
        display.clear();
      }else{
        display.print("NO CAN");
      }
    }

    //Reset by holding both for 2 seconds
    if(digitalRead(DISPLAY1_BUTTON_PIN) == LOW  && digitalRead(DISPLAY2_BUTTON_PIN) == LOW){
      if(!areBothHeld){
        areBothHeld = true;
        bothHeldStart = millis();
      }else if(millis() - bothHeldStart > 2000){
        //Clear everything, reset
        ledMatrix.displaybuffer[1] = 0;
        ledMatrix.displaybuffer[0] = 0;
        ledMatrix.writeDisplay();
        display.clear();
        display2.clear();
        speed.write(SPEED_MIN_ANGLE);
        rpm.write(RPM_MIN_ANGLE);
        delay(500);
        ESP.restart();
      }
      ledMatrix.displaybuffer[1] = ledMatrix.displaybuffer[1] | 0b1000000000;
      ledMatrix.displaybuffer[0] = ledMatrix.displaybuffer[0] | 0b1000000000;
      ledMatrix.writeDisplay();
    }else if(areBothHeld){
      //If they aren't actively both held but we still have the flag, Clear warning
      ledMatrix.displaybuffer[1] = ledMatrix.displaybuffer[1] & 0b1111110111111111;
      ledMatrix.displaybuffer[0] = ledMatrix.displaybuffer[0] & 0b1111110111111111;
      ledMatrix.writeDisplay();
      areBothHeld = false;
    }

    //If there has been CAN recently, display stuff 
    if(tempCANRecentRX){
      //Switch display data field with cooldown (note: active low)
      if(digitalRead(DISPLAY1_BUTTON_PIN) == LOW && millis()-lastDisp1Button > 300){
        lastDisp1Button = millis();
        display1CurrentDisplay++;
        //Rollover
        if(display1CurrentDisplay == END_ELEMENT){
          display1CurrentDisplay = 0;
        }
        //Save to the ESP32
        prefs.begin("DisplayPrefs", false);
        prefs.putInt("Display1", display1CurrentDisplay);
        prefs.end();
      }
      if(digitalRead(DISPLAY2_BUTTON_PIN) == LOW && millis()-lastDisp2Button > 300){
        lastDisp2Button = millis();
        display2CurrentDisplay++;
        //Rollover
        if(display2CurrentDisplay == END_ELEMENT){
          display2CurrentDisplay = 0;
        }
        //Save to the ESP32
        prefs.begin("DisplayPrefs", false);
        prefs.putInt("Display2", display2CurrentDisplay);
        prefs.end();
      }

      //Display the correct thing for the display
      switch(display1CurrentDisplay){
        case CVT_TEMP:
          displayCVTTemp(display);
          break;
        case TIME:
          displayTime(display);
          break;
        case RPM:
          displayRPM(display);
          break;
        case SPEED:
          displaySpeed(display);
          break;
        case DISTANCE:
          displayDistance(display);
          break;
        default:
          display.print("????");
          break;
      }

      switch(display2CurrentDisplay){
        case CVT_TEMP:
          displayCVTTemp(display2);
          break;
        case TIME:
          displayTime(display2);
          break;
        case RPM:
          displayRPM(display2);
          break;
        case SPEED:
          displaySpeed(display2);
          break;
        case DISTANCE: 
          displayDistance(display2);
          break;
        default:
          display2.print("????");
          break;
      }

      //Write LED matrix and RPM/Speed gauges
      uint16_t tempIndicatorLights = indicatorLightState; 
      float tempSpeed = lastSpeed;
      float tempRPM = lastRPM;
      ledMatrix.displaybuffer[0] = tempIndicatorLights;
      ledMatrix.displaybuffer[1] = tempIndicatorLights;
      //Clamp values so the servos don't strain themselves
      if(tempSpeed > 39.5) tempSpeed = 39.5;
      else if(tempSpeed < 0) tempSpeed = 0;
      if(tempRPM > 3800) tempRPM = 3800;
      else if(tempRPM < 0) tempRPM = 0;
      //Formula: ((value/maxValue) * servoRange) + minRange
      speed.write(((tempSpeed / 39.5) * (SPEED_MAX_ANGLE - SPEED_MIN_ANGLE)) + SPEED_MIN_ANGLE);
      rpm.write(((tempRPM / 3800.0) * (RPM_MAX_ANGLE - RPM_MIN_ANGLE)) + RPM_MIN_ANGLE);

      //Put this so the indicator doesn't get overridden
      if(areBothHeld){
        ledMatrix.displaybuffer[1] = ledMatrix.displaybuffer[1] | 0b1000000000;
        ledMatrix.displaybuffer[0] = ledMatrix.displaybuffer[0] | 0b1000000000;
      }
      ledMatrix.writeDisplay(); //Putting this down here so the reset warning doesn't flicker
    }
    //Effectively limits the refresh rate to ~10Hz (which is fine, we don't need much more than 5Hz)
    delay(100); 
  } 
}

//Displays the latest CVT temp information on the given display
void displayCVTTemp(HT16K33 disp){
  float tempCVTTemp = lastCVTTemp;
  disp.printf("CVT %4.0f", tempCVTTemp);
}

//Displays the latest time information on the given display
void displayTime(HT16K33 disp){
  unsigned long realSeconds = (unsigned long)lastTimeSeconds;
  unsigned long currentMinutes = lastTimeSeconds/60;
  unsigned long currentHours = lastTimeSeconds/3600;
  disp.printf("%02u%02u  %02u", currentHours, currentMinutes%60, realSeconds%60);  
  disp.colonOn();
}

//Displays the latest RPM information on the given display
void displayRPM(HT16K33 disp){
  float tempRPM = lastRPM;
  disp.printf("RPM %4.0f", tempRPM);
}

//Displays the latest speed information on the given display
void displaySpeed(HT16K33 disp){
  float tempSpeed = lastSpeed;
  disp.printf("SPD %4.0f", tempSpeed);
}

//Displays the latest distance information on the given display
void displayDistance(HT16K33 disp){
  float tempDistance = lastDistance;
  disp.printf("DST %5.1f", tempDistance);
  disp.decimalOn();
}
