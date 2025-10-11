#ifndef CANTPROTOCOL_H
#define CANTPROTOCOL_H

#include <mcp_can.h>
#include <Arduino.h>
#include <queue>
#include <functional>


template <typename T> class CANTProtocol{
    public:
        CANTProtocol(int cs, int interrupt, byte deviceID) : CAN_CS_PIN(cs), CAN_INTERRUPT_PIN(interrupt), CAN_DEVICE_ID(deviceID), CAN(cs){}
        bool begin();
        bool end();
        
        void registerRequest(byte commandID, std:function<T(byte[] incomingData)> dataBuilder);
        void registerCommand(byte commandID, std:function<void(byte[] incomingData)> onRecieved);
        
        void execute();

    private:
        const int CAN_CS_PIN;
        const int CAN_INTERRUPT_PIN; 
        const byte CAN_DEVICE_ID;
        MCP_CAN CAN;

        void ISR();

        struct RegisteredCANMessage {
            bool isRequest;
            //Only one of these functions will be declared
            std::function<T(byte[] incomingData)> dataBuilder;
            std::function<void(byte[] incomingData)> onRecieved;
        }

        //Initialized to an array of nullptr
        RegisteredCANMessage* registeredMessages[16] = {};

        struct PendingCANRequest{
            byte[8] data; //Recieved data
            unsigned long callbackID;
            std::function<T(byte[] incomingData)> dataBuilder; //
        };

        struct PendingCANCommand{
            byte[8] data; //Recieved data
            unsigned long callbackID;
            std::function<void(byte[] incomingData)> onRecieved; //
        }
        

        volatile PendingCANRequest[16] pendingRequests;
        
        volatile PendingCANCommand[16] pendingCommands;

};


#endif