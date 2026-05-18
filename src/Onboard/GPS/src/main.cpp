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

void builderP(unsigned char dataLength, byte* incomingData, unsigned long callbackID){
    GPSPosition longitude = myGNSS.getLongitude();
    GPSPosition latitude = myGNSS.getLatitude();
    GPSPosition altitude = myGNSS.getAltitude();
    CAN.sendRequestReponse(GPSPosition, callbackID);
}

void builderT(unsigned char dataLength, byte* incomingData, unsigned long callbackID){
    uint32_t time = myGNSS.getUnixEpoch();
    CAN.sendRequestReponse(time, callbackID);
}

void builderR(unsigned char dataLength, byte* incomingData, unsigned long callbackID){
    if (myGNSS.getFixType() < 3){
        CAN.sendRequestReponse(false, callbackID);
    }
    else {
        CAN.sendRequestReponse(true, callbackID);
    }
}

void setup()
{
    while(!CAN.begin()){
      delay(100);
    }
    Wire.begin();
    while(!myGNSS.begin()){ 
        delay(100);
    }
    myGNSS.setI2COutput(COM_TYPE_UBX); //Set the I2C port to output UBX only (turn off NMEA noise)  
    myGNSS.saveConfigSelective(VAL_CFG_SUBSEC_IOPORT); //Save (only) the communications port settings to flash and BBR
    CAN.registerRequest(0, builderP);
    CAN.registerRequest(1, builderT);
    CAN.registerRequest(2, builderR);
}

void loop(){
    CAN.execute();
}