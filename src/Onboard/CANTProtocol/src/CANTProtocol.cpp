#include "CANTProtocol.h"

bool CANTProtocol::begin(){
    byte canInitResult = CAN.begin(MCP_STDEXT, CAN_500KBPS, MCP_8MHZ);
    
    if(canInitResult != CAN_OK){
        return false;
    }

    attatchInterrupt(digitalPinToInterrupt(CAN_INTERRUPT_PIN), ISR, FALLING);
    return true;
}
        
void CANTProtocol::registerRequest(byte commandID, std:function<T(byte[] incomingData)> dataBuilder){

}

void CANTProtocol::registerCommand(byte commandID, std:function<void(byte[] incomingData)> onRecieved){

}

void CANTProtocol::execute(){

}

bool CANTProtocol::end(){

}

void CANTProtocol::ISR(){

}