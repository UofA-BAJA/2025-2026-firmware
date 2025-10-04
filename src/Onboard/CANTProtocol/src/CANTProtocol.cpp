#include "CANTProtocol.h"

bool CANTProtocol::begin(){
    //Check for valid CAN ID
    if(CAN_DEVICE_ID & ~(0b11111) > 0){
        return false;
    }

    //Begin CAN
    byte canInitResult = CAN.begin(MCP_STDEXT, CAN_500KBPS, MCP_8MHZ);
    
    if(canInitResult != CAN_OK){
        return false;
    }

    //Filter to just the commands we want
    CAN.init_Mask(0, 1, 0x1F000000);
    CAN.init_Filt(0, 1, CAN_DEVICE_ID << );
    CAN.init_Filt(1, 1, 0x1F000000);

    CAN.init_Mask(1, 1, 0x1F000000);
    CAN.init_Filt(2, 1, 0x1F000000);
    CAN.init_Filt(3, 1, 0x1F000000);
    CAN.init_Filt(4, 1, 0x1F000000);
    CAN.init_Filt(5, 1, 0x1F000000);


    canInitResult = CAN.setMode(MCP_NORMAL);
    if(canInitResult != MCP2515_OK){
        return false;
    }

    attachInterrupt(digitalPinToInterrupt(CAN_INTERRUPT_PIN), ISR, FALLING);
    return true;
}
        
bool CANTProtocol::registerRequest(byte commandID, std:function<T(byte[] incomingData)> dataBuilder){
    //Check for valid command ID
    if(commandID & ~(0b1111) > 0){
        return false;
    }

    RegisteredCANMessage r;
    r.isRequest = true;
    r.dataBuilder = dataBuilder;

    return true;
}

bool CANTProtocol::registerCommand(byte commandID, std:function<void(byte[] incomingData)> onRecieved){
    //Check for valid command ID
    if(commandID & ~(0b1111) > 0){
        return false;
    }

    RegisteredCANMessage r;
    r.isRequest = false;
    r.onRecieved = onRecieved;
    
    return true;
}

void CANTProtocol::execute(){

}

bool CANTProtocol::end(){
    detachInterrupt(digitalPinToInterrupt(CAN_INTERRUPT_PIN));
    CAN.setMode(MCP_SLEEP);

}

void CANTProtocol::ISR(){
    
    //Interrupt goes low and stays low until all buffers are empty
    while(CAN_MSGAVAIL == CAN.checkReceive()){
        long unsigned int rxId = 0;
        unsigned char len = 0;
        unsigned char rxBuf[8];

        CAN.readMsgBuf(&rxId, &len, rxBuf);
        byte dataType = (byte) ((rxId & 0x00F00000) >> 20); 
        unsigned long canCallback = (rxId & 0x000FFFFF);

        if (registeredMessages[dataType] != nullptr){
            if(registeredMessages[dataType] -> isRequest){
                PendingCANRequest p = {rxBuf, canCallback, registeredMessage[dataType] -> dataBuilder};
                pendingRequests.push_back(p);
            }else{
                PendingCANCommand p = {rxBuf, canCallback, registeredMessage[dataType] -> onRecieved};
                pendingCommands.push_back(p);
            }
        }
    }

    
}