#ifndef CANTPROTOCOL_H
#define CANTPROTOCOL_H

#include <mcp_can.h>
#include <Arduino.h>
#include <map>
#include <functional>


template <typename T> class CANTProtocol{
    public:
        CANTProtocol(int cs, int interrupt) : CAN_CS_PIN(cs), CAN_INTERRUPT_PIN(interrupt), CAN(cs){}
        bool begin();
        bool end();
        
        void registerRequest(byte commandID, std:function<T(byte[] incomingData)> dataBuilder);
        void registerCommand(byte commandID, std:function<void(byte[] incomingData)> onRecieved);
        
        void execute();

    private:
        const int CAN_CS_PIN;
        const int CAN_INTERRUPT_PIN; 
        MCP_CAN CAN;

        void ISR();

        struct RegisteredCANMessage {
            byte commandID;
            bool isRequest;

        }

        std::vector<RegisteredCANMessage> messages;

        struct PendingCANRequest{
            byte[8] data; //The command/data ID (4 bits)
            std::function<T(byte[] incomingData)> dataBuilder; //
        };

        struct PendingCANCommand{
            byte[8] data; //The command/data ID (4 bits)
            std::function<void(byte[] incomingData)> onRecieved; //
        }
        
        std::queue<PendingCANRequest> pendingRequests;
        std::queue<PendingCANCommand> pendingCommands;

};


#endif