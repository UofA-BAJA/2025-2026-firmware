#include <Arduino.h>
#include <Wire.h>
#include <SparkFun_u-blox_GNSS_Arduino_Library.h>
#include "CANTProtocol.h"

CANTProtocol CAN(5,17,0xA); //need to check pins
SFE_UBLOX_GNSS myGNSS;



typedef struct GPSPosition{
    float longitude;
    float latitude;
    float altitude;
}GPSPosition;

SemaphoreHandle_t gpsMutex = NULL;
void readGPS(void *pvParameters);

float readLongitude = 0.0;
float readLatitude = 0.0;
float readAltitude = 0.0;
uint32_t readTime = 0.0;
uint8_t readStatus = 0.0;

void builderP(unsigned char dataLength, byte* incomingData, unsigned long callbackID){
    GPSPosition pos;
    if(xSemaphoreTake(gpsMutex, portMAX_DELAY)){
        pos.longitude = readLongitude;
        pos.latitude = readLatitude;
        pos.altitude = readAltitude;
        xSemaphoreGive(gpsMutex);
    }
    CAN.sendRequestResponse(pos, callbackID);
}

void builderT(unsigned char dataLength, byte* incomingData, unsigned long callbackID){
    uint32_t time = 0;
    if(xSemaphoreTake(gpsMutex, portMAX_DELAY)){
        time = readTime;
        xSemaphoreGive(gpsMutex);
    }
    CAN.sendRequestResponse(time, callbackID);
}

void builderR(unsigned char dataLength, byte* incomingData, unsigned long callbackID){
    uint8_t status = 0;
    if(xSemaphoreTake(gpsMutex, portMAX_DELAY)){
        status = readStatus;
        xSemaphoreGive(gpsMutex);
    }
    // Serial.println(status);
    if (status < 3){
        CAN.sendRequestResponse(false, callbackID);
    }
    else {
        CAN.sendRequestResponse(true, callbackID);
    }
}

void setup()
{
    Serial.begin(115200);
    Wire.begin();
    Wire.setClock(400000);
    while(!myGNSS.begin()){ 
        delay(100);
    }
    myGNSS.setI2COutput(COM_TYPE_UBX); //Set the I2C port to output UBX only (turn off NMEA noise)  
    myGNSS.saveConfigSelective(VAL_CFG_SUBSEC_IOPORT); //Save (only) the communications port settings to flash and BBR
    CAN.registerRequest(0, builderP);
    CAN.registerRequest(1, builderT);
    CAN.registerRequest(2, builderR);
    
    
    gpsMutex = xSemaphoreCreateMutex();

    xTaskCreate(
        readGPS,
        "Read GPS",
        4096,
        NULL,
        10,
        NULL
    );

    while(!CAN.begin()){
      delay(100);
    }
}

void loop(){
    CAN.execute();
}

void readGPS(void *pvParameters){
    float tempLongitude = 0.0;
    float tempLatitude = 0.0;
    float tempAltitude = 0.0;
    uint32_t tempTime = 0.0;
    uint8_t tempStatus = 0.0;
    while (true){
        tempLongitude = myGNSS.getLongitude();
        tempLatitude = myGNSS.getLatitude();
        tempAltitude = myGNSS.getAltitude();
        tempTime = myGNSS.getUnixEpoch();
        tempStatus = myGNSS.getFixType();
        // Serial.println(tempStatus);
        if(xSemaphoreTake(gpsMutex, portMAX_DELAY)){
            readLongitude = tempLongitude;
            readLatitude = tempLatitude;
            readAltitude = tempAltitude;
            readTime = tempTime;
            readStatus = tempStatus;
            xSemaphoreGive(gpsMutex);
        }
        delay(30);
    }
}