//new can test

#include <mcp_can.h>
#include <SPI.h>
#include <Wire.h>
// #include <MLX90614.h>
#include <Arduino.h>
#include "CANTProtocol.h"
#include <functional>


const int SPI_CS_PIN = 10; // CS pin for the MLX90614
// MCP_CAN CAN(SPI_CS_PIN);  // Create CAN object on CS pin

CANTProtocol CAN(SPI_CS_PIN, 2, 0x1F);

typedef struct TestStruct_s{
    float a;
    float b;
    float c;
    float d;
    float e;
} TestStruct;

// MLX90614 mlx = MLX90614(MLX90614_BROADCASTADDR);


void setup()
{
    Wire.begin();
    Serial.begin(115200);

    bool canInitResult = CAN.begin();

    if(!canInitResult){
        Serial.println("CAN Init failed!!!!!");
        while (1) ;
    }
    
    
    //---------------------------------------------------

    delay(1000);

    Serial.println("Init OK!");
}

// byte data[8] = {0x01, 0x01, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09};


void loop()
{

      
}

