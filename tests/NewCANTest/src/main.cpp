//new can test

#include <mcp_can.h>
#include <SPI.h>
#include <Wire.h>
// #include <MLX90614.h>
#include <Arduino.h>

const int SPI_CS_PIN = 10; // CS pin for the MLX90614
MCP_CAN CAN(SPI_CS_PIN);  // Create CAN object on CS pin

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

    byte canInitResult = CAN.begin(MCP_STDEXT, CAN_500KBPS, MCP_8MHZ);

    if (canInitResult == CAN_OK)
    {
        Serial.println("CAN Init OK!");
    }
    else if (canInitResult == CAN_FAILINIT)
    {
        Serial.println("CAN Init Failed: CAN_FAILINIT");
        Serial.end();
        while (1)
        ;
    }
    else if (canInitResult == CAN_FAILTX)
    {
        Serial.println("CAN Init Failed: CAN_FAILTX");
        Serial.end();
        while (1)
        ;
    }
    else
    {
        Serial.println("CAN Init Failed: Unknown error");
        Serial.end();
        while (1)
        ;
    }

    CAN.init_Mask(0, 1, 0x1F000000);
    CAN.init_Filt(0, 1, 0x1F000000);
    CAN.init_Filt(1, 1, 0x1F000000);

    CAN.init_Mask(1, 1, 0x1F000000);
    CAN.init_Filt(2, 1, 0x1F000000);
    CAN.init_Filt(3, 1, 0x1F000000);
    CAN.init_Filt(4, 1, 0x1F000000);
    CAN.init_Filt(5, 1, 0x1F000000);



    // Set the MCP2515 to normal mode to start receiving CAN messages
    Serial.println("Setting CAN Normal");
    CAN.setMode(MCP_NORMAL);
    
    
    //---------------------------------------------------

    delay(1000);

    Serial.println("Init OK!");
}

// byte data[8] = {0x01, 0x01, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09};


void loop()
{
    long unsigned int rxId = 0;
    unsigned char len = 0;
    unsigned char rxBuf[8];



  
    // Check for incoming CAN messages
    while (CAN_MSGAVAIL == CAN.checkReceive()){
        float onBoardTemp = 69;
        CAN.readMsgBuf(&rxId, &len, rxBuf); // Read message
        byte dataType = (byte) ((rxId & 0x00F00000) >> 20); 
        unsigned long canCallback = (rxId & 0x000FFFFF);
        Serial.print("Data Type: ");
        Serial.println(dataType, HEX);
        Serial.print("CAN Callback: ");
        Serial.println(canCallback, HEX);
        // Serial.print(">ID: ");
        // Serial.println(rxId, HEX);

        // for (int i = 0; i < len; i++) {
        //   Serial.print(">Data: ");
        //   Serial.println(rxBuf[i], HEX);
        // }
    

      

        if (dataType == 0x01){
            //Request
            if(canCallback > 0){
                TestStruct e;
                e.a = onBoardTemp;
                e.b = onBoardTemp + 1;
                e.c = onBoardTemp + 2;
                e.d = onBoardTemp + 3;
                e.e = onBoardTemp + 4;

                byte part1[8];
                byte part2[8];
                byte part3[4];
                byte* structPtr = (byte*) &e;
                memcpy(&part1, structPtr, 8);                
                memcpy(&part2, structPtr + 8, 8);
                memcpy(&part3, structPtr + 16, 4);

                byte sendMSG = CAN.sendMsgBuf(canCallback, 1, 8, part1);
                if(sendMSG != CAN_OK){
                    Serial.print("Error Sending Message 1...");
                    Serial.println(sendMSG);
                }
                sendMSG = CAN.sendMsgBuf(canCallback+1, 1, 8, part2);
                if(sendMSG != CAN_OK){
                    Serial.print("Error Sending Message 2...");
                    Serial.println(sendMSG);
                }
                sendMSG = CAN.sendMsgBuf(canCallback+2, 1, 4, part3);
                if(sendMSG != CAN_OK){
                    Serial.print("Error Sending Message 3...");
                    Serial.println(sendMSG);
                }
            }
        }else if(dataType == 0x02){
            //Command (lossy/lossless)
            if(canCallback > 0){
                byte r[1];
                r[0] = 0xAF;

                byte sendMSG = CAN.sendMsgBuf(canCallback, 1, 8, r);
                if(sendMSG != CAN_OK){
                    Serial.print("Error Sending Message...");
                    Serial.println(sendMSG);
                }
            }else{
                Serial.println("Lossy 0x02 recieved");
            }
        }
        Serial.println("-------------------------");
  }
}

