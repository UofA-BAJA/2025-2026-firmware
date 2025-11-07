#ifndef CANTPROTOCOL_H
#define CANTPROTOCOL_H

#include <mcp_can.h>
#include <Arduino.h>


class CANTProtocol{
    public:
        CANTProtocol(int cs, int interrupt, unsigned long deviceID) : CAN_CS_PIN(cs), CAN_INTERRUPT_PIN(interrupt), CAN_DEVICE_ID(deviceID), CAN(cs){}
        bool begin();
        bool end();
        

        //this sucks
        //See https://stackoverflow.com/questions/495021/why-can-templates-only-be-implemented-in-the-header-file
        template <typename T>
        bool registerRequest(byte commandID, T (*dataBuilder) (unsigned char dataLength, byte* incomingData)){
            //Check for valid command ID
            if(commandID & ~(0b1111) > 0){
                return false;
            }

            RegisteredRequest<T> r;
            r.dataBuilder = dataBuilder;
            r.CAN = &(this->CAN);

            registeredMessages[commandID] = &r;

            return true;
        };


        bool registerCommand(byte commandID, void (*onRecieved) (unsigned char dataLength, byte* incomingData));
        
        void execute();

        static void ISRHandler(CANTProtocol* ref);
        void InterruptSubroutine();

        MCP_CAN CAN;

    private:
        const int CAN_CS_PIN;
        const int CAN_INTERRUPT_PIN; 
        const unsigned long CAN_DEVICE_ID;
        

        
        

        //Chat-assisted code begins here
        /*
            Basically, in order to have one function to call, but multiple different implementations (one including a template) in the same array,
            you have to have a base struct, and then structs that derive from that base struct.
            call() carries out all the functions, including calling the dataBuilder()/onRecieved() and sending out CAN frames, if nessecary.
            This kinda sucks being in the header file, in the future I might figure out if there's a way to split this off into sub files
            without it breaking the templates (see above: registerRequest())
        */
        struct RegisteredBase {
            virtual ~RegisteredBase() {}
            virtual void call(unsigned char dataLength, byte* incomingData, unsigned long callbackID) = 0;
            virtual bool isRequest() = 0;
            MCP_CAN* CAN; //Need to pass a reference to the CAN object to use CAN in here
        };

        struct RegisteredCommand : RegisteredBase {
            void (*onRecieved) (unsigned char dataLength, byte* incomingData);

            void call(unsigned char dataLength, byte* incomingData, unsigned long callbackID) override{
                if(onRecieved) onRecieved(dataLength, incomingData);
                if(CAN == nullptr) return;

                //If there is a callback, send ack bit. Otherwise do nothing
                if(callbackID > 0){
                    byte outputBuffer[8];
                    outputBuffer[0] = 0x69; //TODO: PROPERLY DEFINE THE ACK BIT
                    byte sendMSG = CAN->sendMsgBuf(callbackID, 1, 1, outputBuffer);

                }
            }

            bool isRequest() override {return false;}
        };

        template <typename T>
        struct RegisteredRequest : RegisteredBase {
            T (*dataBuilder) (unsigned char dataLength, byte* incomingData); 
            
            void call(unsigned char dataLength, byte* incomingData, unsigned long callbackID) override{
                if(!dataBuilder) return;
                T data = dataBuilder(dataLength, incomingData);
                if(CAN == nullptr) return;
                if(callbackID > 0){
                    byte outputBuffer[8];
                    //If we need to send multiple frames back
                    if(sizeof(T) > 8){
                        int numFrames = (sizeof(T) / 8) + 1;
                        byte* ptr = (byte*) &data; //Get a pointer that increments by bytes
                        for(int j = 0; j < numFrames; j++){
                            //(this could be one line but I was having trouble thinking of the right way to do it)
                            //If this is the last frame, memcpy the right amount
                            if(j == numFrames - 1){
                                memcpy(&outputBuffer, ptr + (j * 8), sizeof(T) - (j * 8));
                                byte sendMSG = CAN->sendMsgBuf(callbackID + j, 1, sizeof(T) - (j * 8), outputBuffer);
                            }
                            //Otherwise, copy 8 bytes
                            else {
                                memcpy(&outputBuffer, ptr + (j * 8), 8);
                                byte sendMSG = CAN->sendMsgBuf(callbackID + j, 1, 8, outputBuffer);
                            }
                            //TODO: do something if it fails to send 
                        }
                    }
                }
            }

            bool isRequest() override {return true;}
        };

        //A list of all the messages we can recieve. Each array index is a datatype as defined in the CANT spec 
        RegisteredBase* registeredMessages[16] = {nullptr};
        //Chat-assisted code ends here

 
        //This struct holds the data and callback ID for pending requests and commands
        struct PendingCANFrame{
            byte data[8]; //Recieved data
            unsigned char dataLength;
            unsigned long dataID;
            unsigned long callbackID;
        };
        
        /*
            These implement 3 queues for pending CAN stuff.
            The ISR has to be incredibly short and simple, so it copies critical data into pendingFrames. 
            pendingFrames basically acts like an extended recieve buffer.

            In execute(), these pending frames are copied to queues for requests and commands. 
            This allows us granular control over how many requests and commands can be executed per cycle.
            For example, many requests taking a long time to fulfill can't fully overpower critical commands coming in.
            We could have pendingFrames be the only buffer, but if we got, say, 5 requests in a row and then some critical
            command comes in, we'd have to sift through all 5 requests first. With this system, we can only sift through, say,
            2 of those requests before moving on to sift through many commands. 
        */
        volatile PendingCANFrame pendingFrames[32];
        volatile int frameQueueFront = 0;
        volatile int frameQueueLength = 0;

        PendingCANFrame pendingRequests[32];
        int requestQueueFront = 0;
        int requestQueueLength = 0;
        PendingCANFrame pendingCommands[32];
        int commandQueueFront = 0;
        int commandQueueLength = 0;

};






#endif