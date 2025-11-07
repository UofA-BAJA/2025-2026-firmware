#include "CANTProtocol.h"
#define DEBUG_CAN 1

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
    #if DEBUG_CAN
        Serial.println(CAN_DEVICE_ID << 24, BIN);
    #endif
    //Filter to just the commands we want
    CAN.init_Mask(0, 1, 0x1F000000);
    CAN.init_Filt(0, 1, CAN_DEVICE_ID << 24);
    CAN.init_Filt(1, 1, CAN_DEVICE_ID << 24);

    CAN.init_Mask(1, 1, 0x1F000000);
    CAN.init_Filt(2, 1, CAN_DEVICE_ID << 24);
    CAN.init_Filt(3, 1, CAN_DEVICE_ID << 24);
    CAN.init_Filt(4, 1, CAN_DEVICE_ID << 24);
    CAN.init_Filt(5, 1, CAN_DEVICE_ID << 24);


    canInitResult = CAN.setMode(MCP_NORMAL);
    if(canInitResult != MCP2515_OK){
        return false;
    }

    pinMode(CAN_INTERRUPT_PIN, INPUT);

    //having to pass in this as a parameter to the function sucks!!!!!!
    attachInterrupt(digitalPinToInterrupt(CAN_INTERRUPT_PIN), ISRHandler, FALLING, this);
    return true;
}

//registerRequest() is currently implemented in CANTProtocol.h

bool CANTProtocol::registerCommand(byte commandID, void (*onRecieved) (unsigned char dataLength, byte* incomingData) ){
    //Check for valid command ID
    if(commandID & ~(0b1111) > 0){
        return false;
    }

    RegisteredCommand r;
    r.onRecieved = onRecieved;
    r.CAN = &(this->CAN);

    registeredMessages[commandID] = r;

    return true;
}


//Call in the loop() function to execute pending CAN requests and commands
void CANTProtocol::execute(){
    //Sort all the incoming CAN frames into their respective queues 
    while(frameQueueLength > 0){
        PendingCANFrame f;
        memcpy(f.data, pendingFrames[frameQueueFront].data, pendingFrames[frameQueueFront].dataLength);
        f.dataLength = pendingFrames[frameQueueFront].dataLength;
        f.callbackID = pendingFrames[frameQueueFront].callbackID;
        f.dataID = pendingFrames[frameQueueFront].dataID;
        if(registeredMessages[f.dataID] != nullptr){
            //Segfault or something similar at this line:
            registeredMessages[f.dataID]->isRequest();
            // if(){
        //         //If the length of the queue is 32 frames or greater (max size), drop it. 
        //         if(requestQueueFront > 31) break;

        //         int insertLocation = requestQueueFront + requestQueueLength;
        //         //If we're trying to insert at an index off the end of the array, wraparound to the front
        //         if(insertLocation > 31) insertLocation = insertLocation - 32;

        //         pendingRequests[insertLocation] = f;
        //         requestQueueLength++;
        //     }else{
        //         //If the length of the queue is 32 frames or greater (max size), drop it. 
        //         if(commandQueueFront > 31) break;

        //         int insertLocation = commandQueueFront + commandQueueLength;
        //         //If we're trying to insert at an index off the end of the array, wraparound to the front
        //         if(insertLocation > 31) insertLocation = insertLocation - 32;

        //         pendingCommands[insertLocation] = f;
        //         commandQueueLength++;
            // }
        }
        // frameQueueFront++;
        // frameQueueLength--;
        // if(frameQueueFront == 32) frameQueueFront = 0;
    }


    //Execute up to 2 pending requests
    for(int i = 0; i < 2; i++){
        if(requestQueueLength > 0){
            #if DEBUG_CAN
                Serial.println("executing requests");
            #endif
            PendingCANFrame* r = &pendingRequests[requestQueueFront];
            requestQueueFront++;
            requestQueueLength--;
            if(requestQueueFront == 32) requestQueueFront = 0;

            RegisteredBase* request = registeredMessages[r->dataID];
            if(request != nullptr){
                //Handler code is in the header because cringe
                request->call(r->dataLength, r->data, r->callbackID);
            }
            
        }
    }

    //Execute up to 6 pending commands
    for(int i = 0; i < 6; i++){
        if(commandQueueLength > 0){
            #if DEBUG_CAN
                Serial.println("executing commnads");
            #endif
            PendingCANFrame* r = &pendingCommands[commandQueueFront];
            commandQueueFront++;
            commandQueueLength--;
            if(commandQueueFront == 32) commandQueueFront = 0;
            
            RegisteredBase* command = registeredMessages[r->dataID];
            if(command != nullptr){
                command->call(r->dataLength, r->data, r->callbackID);
            }
        }
    }

}

bool CANTProtocol::end(){
    detachInterrupt(digitalPinToInterrupt(CAN_INTERRUPT_PIN));
    CAN.setMode(MCP_SLEEP);
}

//ISRs have to be static functions. This calls the ISR of a specific instance of the CANT protocol
//This does currently limit the amount of independent CAN controllers a device can have to 1, shouldn't be an issue for 2025-26
static void CANTProtocol::ISRHandler(CANTProtocol* ref){
    ref->InterruptSubroutine();
}

//DON'T CALL THIS YOURSELF. THIS WILL MESS THINGS UP
void CANTProtocol::InterruptSubroutine(){
    noInterrupts();
    long unsigned int rxId = 0;
    unsigned char len = 0;
    unsigned char rxBuf[8];

    //Interrupt goes low and stays low until all buffers are empty
    while(CAN.readMsgBuf(&rxId, &len, rxBuf) != CAN_NOMSG){
        PendingCANFrame p;
        memcpy(&p.data, &rxBuf, len);
        p.dataLength = len;
        p.callbackID = (rxId & 0x000FFFFF);
        p.dataID = ((rxId & 0x00F00000) >> 20); 

        //If the length of the queue is 32 frames or greater (max size), drop it. 
        if(frameQueueLength > 31) break;

        int insertLocation = frameQueueFront + frameQueueLength;
        //If we're trying to insert at an index off the end of the array, wraparound to the front
        if(insertLocation > 31) insertLocation = insertLocation - 32;

        pendingFrames[frameQueueFront + frameQueueLength] = p;
        frameQueueLength++;
    }
    interrupts();
}