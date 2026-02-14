/*

comments
comments
comments


*/

#define RADIO_CS_PIN 11
#define RADIO_INT_PIN 21

#include "Coms.h"

namespace BajaWildcatRacing {

    Coms::Coms(ProcedureScheduler& procedureScheduler)
    :rf95(RADIO_CS_PIN, RADIO_INT_PIN)
    ,procedureScheduler(procedureScheduler) {
        radioThread = std::thread(&Coms::executeRadio, this);
    }


    void Coms::execute(float timestamp){
        //All this does is updates the timestamp
        //Actual sending is done in executeRadio() so it's a different thread
        currTimestamp = timestamp; //Protected by atomic
    }

    void Coms::end(){

    }

    void Coms::executeRadio(){
        //init radio 
        if (!rf95.init()){
            std::cout << "RF95 is not responding!!!" << std::endl;
            //TODO: log error/panic
            running = false;
            return;
        }

        rf95.setFrequency(915.0);
        rf95.setTxPower(23, false); //Sets maximum power for the LoRa module

        //TODO: define what the addresses we are using are
        rf95.setThisAddress(1);
        rf95.setHeaderFrom(1);
        rf95.setHeaderTo(2);
    

        int waitTimems = (1.0 / RADIO_CLOCK_FREQUENCY) * 1000;
        while(running){
            if(currentPitCommandState == PitCommandState::IDLE){
                idle();
            }
            else if(currentPitCommandState == PitCommandState::LIVE_DATA_TRANSMIT){
                transmitLiveData();
            }
            else if(currentPitCommandState == PitCommandState::WAIT_COMMAND_RECIEVE){
                recieveCommand();
            }

            //TODO: make this dynamic wait like the main loop
            std::this_thread::sleep_for(std::chrono::milliseconds(waitTimems));
        }
        //run correct function depending on state
        //if live_data_transmit, send special packet and switch to recievecommand once every second 
    }

    void Coms::transmitLiveData(){
        //take stuff from queue and transmit as fast as possible
        std::unique_lock<std::mutex> lock(dataQueueMutex);
        //Quickly swap the queues so that stuff can still be queued while sending
        std::queue<DataFrame> localQueue;
        std::swap(localQueue, queuedData);

        //Clear the queued data pointers
        for(int i = 0; i < 256; i++){
            queuedDataPointers[i] = nullptr;
        }
        lock.unlock();

        while(!localQueue.empty()){
            DataFrame nextFrame = localQueue.front();
            localQueue.pop();

            //Add timestamp and actual data to data to send
            int sentDataLength = sizeof(float) + nextFrame.dataLength;
            byte data[sentDataLength];
            memcpy(data, &nextFrame.timestamp, sizeof(float));
            memcpy(&data[sizeof(float)], nextFrame.data.get(), nextFrame.dataLength);

            //todo: set the flag for the switchover if needed
            rf95.setHeaderFlags(0x00);
            rf95.setHeaderId(nextFrame.id);

            //Actually send the data
            if(!rf95.send(data, sizeof(sentDataLength))){
                //Failed to send
                std::cout << "Failed to send data with id " << nextFrame.id << std::endl;
            }
        }

    }

    void Coms::recieveCommand(){
        //check for RX on radio with short timeout
        //do procedurescheduler command / radio command (or nothing)
        //switch back to LIVE_DATA_TRANSMIT (or IDLE if command is to go to IDLE)
    }

    void Coms::idle(){
        //listen for commands
        //this is different to setting running = false 
    }

    void Coms::sendData(DataTypes dataType, byte data[], int dataLength){
        //Only send if less than 256
        if(dataType >= 256){
            //Print for debugging, should be resolved before actual use
            std::cout << "Error: Cannot send datatype >= 256 over radio! (attempted to send " << dataType << ")" << std::endl;
            return;
        }

        if(dataLength >= 252){
            //Print for debugging, should be resolved before actual use
            std::cout << "Error: Cannot send data with length >= 252 over radio! (attempted to send data length " << dataLength << ")" << std::endl;
            return;
        }

        //Do we want to send this data?
        if(!dataTypeMask[dataType]){
            //Don't print anything as this will frequently happen in live events
            return;
        }

        std::unique_lock<std::mutex> lock(dataQueueMutex);
        if(queuedDataPointers[dataType] == nullptr){
            //Build up the new frame
            DataFrame newFrame;
            newFrame.id = dataType;
            newFrame.timestamp = currTimestamp;
            newFrame.data = std::make_unique<byte[]>(dataLength);
            memcpy(newFrame.data.get(), data, dataLength);
            newFrame.dataLength = dataLength;

            //Push onto queue
            queuedData.push(newFrame);

            //Add to pointers list
            queuedDataPointers[dataType] = std::make_shared<DataFrame>(newFrame);
        }else{
            //Overwrite the current thing in the queue, rather than adding a new thing
            queuedDataPointers[dataType]->id = dataType;
            queuedDataPointers[dataType]->timestamp = currTimestamp;
            queuedDataPointers[dataType]->data = std::make_unique<byte[]>(dataLength);
            memcpy(queuedDataPointers[dataType]->data.get(), data, dataLength);
            queuedDataPointers[dataType]->dataLength = dataLength;
        }
        lock.unlock();
    }
}