//new can test

#include <mcp_can.h>
#include <SPI.h>
#include <Wire.h>
// #include <MLX90614.h>
#include <Arduino.h>
#include "CANTProtocol.h"



const int SPI_CS_PIN = 10; // CS pin for the MLX90614
// MCP_CAN CAN(SPI_CS_PIN);  // Create CAN object on CS pin

CANTProtocol CAN(SPI_CS_PIN, 2, 0x1Fl);

typedef struct TestStruct_s{
    float a;
    float b;
    float c;
    float d;
    float e;
} TestStruct;

// MLX90614 mlx = MLX90614(MLX90614_BROADCASTADDR);

TestStruct builderA(unsigned char dataLength, byte* incomingData){
    return TestStruct{69.0, 70.0, 71.0, 72.0, 73.0};
}


void setup()
{
    Wire.begin();
    Serial.begin(115200);

    bool requestReg = CAN.registerRequest(0x01, builderA);
    if(!requestReg) Serial.println("Request registriation failed!!!");

    bool canInitResult = CAN.begin();


    if(!canInitResult){
        Serial.println("CAN Init failed!!!!!");
        while (1) ;
    }
    
    // CAN.begin(MCP_STDEXT, CAN_500KBPS, MCP_8MHZ);
    // unsigned long CAN_ID = (0x1Fl << 24);
    // CAN.init_Mask(0, 1, 0x1F000000);
    // CAN.init_Filt(0, 1, CAN_ID);
    // CAN.init_Filt(1, 1, CAN_ID);

    // CAN.init_Mask(1, 1, 0x1F000000);
    // CAN.init_Filt(2, 1, CAN_ID);
    // CAN.init_Filt(3, 1, CAN_ID);
    // CAN.init_Filt(4, 1, CAN_ID);
    // CAN.init_Filt(5, 1, CAN_ID);

    // CAN.init_Mask(0, 1, 0x1F000000);
    // CAN.init_Filt(0, 1, 0x1F000000);
    // CAN.init_Filt(1, 1, 0x1F000000);

    // CAN.init_Mask(1, 1, 0x1F000000);
    // CAN.init_Filt(2, 1, 0x1F000000);
    // CAN.init_Filt(3, 1, 0x1F000000);
    // CAN.init_Filt(4, 1, 0x1F000000);
    // CAN.init_Filt(5, 1, 0x1F000000);
    
    // Serial.println(CAN_ID, BIN);
    // CAN.setMode(MCP_NORMAL);

    //---------------------------------------------------

    delay(1000);

    Serial.println("Init OK!");
}

// byte data[8] = {0x01, 0x01, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09};


void loop()
{
    CAN.execute();
    // long unsigned int rxId = 0;
    // unsigned char len = 0;
    // unsigned char rxBuf[8];
    // while(CAN.checkReceive() == CAN_MSGAVAIL){
    //     CAN.readMsgBuf(&rxId, &len, rxBuf);
    //     Serial.println(rxId, BIN);

    // }
      
}

