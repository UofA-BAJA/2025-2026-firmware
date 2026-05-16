#include <mcp_can.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <utility/imumaths.h>
#include <Arduino.h>
#include "CANTProtocol.h"

typedef struct rotation {
  float xRot;
  float yRot;
  float zRot;
} rotation;

typedef struct acceleration {
  float xAcc;
  float yAcc;
  float zAcc;
} acceleration;

float xRot = 0;
float yRot = 0;
float zRot = 0;
float xAcc = 0;
float yAcc = 0;
float zAcc = 0;

//5 is IO5, 17 is IO17
CANTProtocol CAN(5, 17, 0x1);

SemaphoreHandle_t bnoMutex = NULL;

void builderRotation(unsigned char dataLength, byte* incomingData, unsigned long callbackID) {
  rotation theRotation;
  if(xSemaphoreTake(bnoMutex, portMAX_DELAY)){
    theRotation.xRot = xRot;
    theRotation.yRot = yRot;
    theRotation.zRot = zRot;
    xSemaphoreGive(bnoMutex);
  }

  CAN.sendRequestResponse(theRotation, callbackID);

}

void builderAcceleration(unsigned char dataLength, byte* incomingData, unsigned long callbackID) {
  acceleration theAcceleration;
  if(xSemaphoreTake(bnoMutex, portMAX_DELAY)){
    theAcceleration.xAcc = xAcc;
    theAcceleration.yAcc = yAcc;
    theAcceleration.zAcc = zAcc;
    xSemaphoreGive(bnoMutex);
  }

  CAN.sendRequestResponse(theAcceleration, callbackID);

}

Adafruit_BNO055 bno = Adafruit_BNO055(55);

void readBNO(void *pvParameters);

void setup()
{
  Serial.begin(115200);
  CAN.registerRequest(0, builderRotation);
  CAN.registerRequest(1, builderAcceleration);
  while(!CAN.begin()) {
    delay(100);
  }

  //---------------------------------------------------
  // Create mutex O.o crazy comment right here
  bnoMutex = xSemaphoreCreateMutex();

  xTaskCreate(
    readBNO,
    "Read BNO",
    4096,
    NULL,
    10,
    NULL
  );

 Serial.println("Init OK!");
}

// byte data[8] = {0x01, 0x01, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09};

void loop()
{
  CAN.execute();
}

void readBNO(void *pvParameters){

  /* Initialise the sensor */
  Serial.println("BNO Init");
  if (!bno.begin())
  {
    /* There was a problem detecting the BNO055 ... check your connections */
    Serial.print("Ooops, no BNO055 detected ... Check your wiring or I2C ADDR!");
    while (1)
      ;
  }
  // bno.setMode(OPERATION_MODE_GYRONLY);

  bno.setExtCrystalUse(true);
  Serial.println("BNO Init OK!");
  Wire.setClock(400000);

  while(true){
      sensors_event_t event;
      sensors_event_t accelerationEvent;
    
      bno.getEvent(&event);
      bno.getEvent(&accelerationEvent, Adafruit_BNO055::VECTOR_LINEARACCEL);
    if(xSemaphoreTake(bnoMutex, portMAX_DELAY)){
      xRot = event.orientation.x;
      yRot = event.orientation.y;
      zRot = event.orientation.z;
      xAcc = accelerationEvent.acceleration.x;
      yAcc = accelerationEvent.acceleration.y;
      zAcc = accelerationEvent.acceleration.z;

      xSemaphoreGive(bnoMutex);
    }

    delay(30);
  }
}