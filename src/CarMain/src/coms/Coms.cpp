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

        std::fill(dataTypeMask, dataTypeMask+32, 0xFF);
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
        std::cout << "RF95 init..." << std::endl;
        if (!rf95.init()){
            std::cout << "RF95 is not responding!!!" << std::endl;
            //TODO: log error/panic
            running = false;
            return;
        }

        rf95.setFrequency(915.0);
        rf95.setTxPower(23, false); //Selects maximum power for the LoRa module
        std::cout << "device version: " << rf95.spiRead(0x42) << std::endl;

        //TODO: define what the addresses we are using are
        // rf95.setThisAddress(1);
        // rf95.setHeaderFrom(1);
        // rf95.setHeaderTo(2);
    
        std::cout << "RF95 initialized!!!" << std::endl;

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
        std::queue<std::shared_ptr<DataFrame>> localQueue;
        std::swap(localQueue, queuedData);

        //Clear the queued data pointers
        for(int i = 0; i < 256; i++){
            queuedDataPointers[i] = nullptr;
        }
        lock.unlock();

        //Start building up the array
        byte data[RH_RF95_MAX_MESSAGE_LEN];
        int sentDataLength = 0;
        int dataStartPos = 0;

        while(!localQueue.empty()){
            std::shared_ptr<DataFrame> nextFrame = localQueue.front();
            localQueue.pop();
            // std::cout << "Sending frame with id " << nextFrame->id << std::endl;

            int nextSize = 1 + sizeof(float) + nextFrame->dataLength;

            //If the next data is about to go past over the max length, send what we have out
            if(sentDataLength + nextSize > RH_RF95_MAX_MESSAGE_LEN){
                std::cout << "Sending data now! Not done this cycle. Size: " << sentDataLength << std::endl;
                radioTransmit(data, sentDataLength);
                sentDataLength = 0;
                dataStartPos = 0;
            }

            //Add ID (1 byte), timestamp (float) and actual data to data to send
            sentDataLength += nextSize;

            //Copy ID, timestamp, data
            memcpy(data + dataStartPos, &nextFrame->id, 1);
            dataStartPos += 1;
            memcpy(data + dataStartPos, &nextFrame->timestamp, sizeof(float));
            dataStartPos += sizeof(float);
            memcpy(data + dataStartPos, nextFrame->data.get(), nextFrame->dataLength);
            dataStartPos += nextFrame->dataLength;
        }

        //Send final packet if there's actually data there
        if(sentDataLength > 0){
            std::cout << "Sending data now! Size: " << sentDataLength << std::endl;
            radioTransmit(data, sentDataLength);
            sentDataLength = 0;
        }

    }

    void Coms::radioTransmit(byte data[], int sentDataLength){
        //If the mode isn't in idle mode (edge case check)
        if(rf95.mode() != RH_RF95::RHModeIdle){

            //Wait up to 50 ms for it to be done
            uint8_t irqFlags = rf95.spiRead(RH_RF95_REG_12_IRQ_FLAGS);
            int repeats = 0;
            while (!(irqFlags & RH_RF95_TX_DONE) && repeats < 100){
                // std::cout << "repeat " << repeats << std::endl;
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
                repeats++;
                irqFlags = rf95.spiRead(RH_RF95_REG_12_IRQ_FLAGS);
                
            }

            //Still not done??? Timeout
            if(repeats >= 100){
                std::cout << "TIMEOUT Radio TX" << std::endl;
                return;
            }
            else{
                //Clear the flags
                rf95.spiWrite(RH_RF95_REG_12_IRQ_FLAGS, RH_RF95_TX_DONE);
                // std::this_thread::sleep_for(std::chrono::milliseconds(5));
                // std::cout << "Cleared flags" << std::endl;

                //Set the mode to idle
                rf95.setModeIdle();
                //Wait a brief amount of time for the mode to change
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }
        }

        //Set flags appropriately 
        //TODO: actually set them based on recieve flag or switch flag
        rf95.setHeaderFlags(0x00);

        // std::cout << "about to send the data" << std::endl;
        if(!rf95.send(data, sizeof(sentDataLength))){
            //Failed to send
            std::cout << "Failed to send data frame." << std::endl;
        }
    }

    void Coms::recieveCommand(){
        //to do: better way to do this so it doesn't mess with the rest of the timing?
        if(rf95.waitAvailableTimeout(100)){
            uint8_t rxBuffer[RH_RF95_MAX_MESSAGE_LEN]; 
            uint8_t rxLength = sizeof(rxBuffer);
            if(rf95.recv(rxBuffer, &rxLength)){
                rxSuccessful = true;
                currentPitCommandState = PitCommandState::LIVE_DATA_TRANSMIT;
                
                if(rxLength = 0){
                    //Zero length: no commands, go back to live data transmit, set flag
                    return;
                }else{
                    //Actual command recieved
                    for(int i = 0; i < rxLength; i++){
                        if(rxBuffer[i] == 0){
                            //Change frequency 
                            //TODO: how are we encoding this lmao
                        }else if(rxBuffer[i] == 1){
                            //Change commands
                            i++;
                            if(rxLength - i < 32){
                                //Not enough i's left 
                                std::cerr << "Error: recieved too short of a frame for the data change command" << std::endl;
                                return;
                            }
                        
                            memcpy(dataTypeMask, rxBuffer, 32);
                            i += 31; //don't want to skip over the end, only skip 31 places
                        }else{
                            //Non-special command
                            procedureScheduler.receiveComCommand((Command)rxBuffer[i]);
                        }
                    }
                }
            }
        }
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

        if(dataLength >= RH_RF95_MAX_MESSAGE_LEN - 1 - sizeof(float)){
            //Print for debugging, should be resolved before actual use
            std::cout << "Error: Cannot send data with length >= 247 over radio! (attempted to send data length " << dataLength << ", datatype " << dataType << ")" << std::endl;
            return;
        }

        if(data == nullptr){
            std::cout << "Error: data cannot be null when sending to radio (datatype " << dataType << ")" << std::endl;
            return;
        }

        //Do we want to send this data?
        //The LSB is the lower number (even though it's to the "right" when we write it down)
        //This complicated mess basically finds the right spot
        if(!(dataTypeMask[dataType % 8] & (1 << dataType % 8))){
            //Don't print anything as this will frequently happen in live events
            //Temporary do print something until the radio is working
            std::cout << "Not sending this data of datatype " << dataType << std::endl;
            return;
        }

        std::unique_lock<std::mutex> lock(dataQueueMutex);
        if(queuedDataPointers[dataType] == nullptr){
            //Build up the new frame
            auto newFrame = std::make_shared<DataFrame>();
            newFrame->id = dataType;
            newFrame->timestamp = currTimestamp;
            std::shared_ptr<byte[]> sp(new byte[dataLength]);
            newFrame->data = sp;
            memcpy(newFrame->data.get(), data, dataLength);
            newFrame->dataLength = dataLength;

            //Push onto queue
            queuedData.push(newFrame);

            //Add to pointers list
            queuedDataPointers[dataType] = newFrame;
            // std::cout << "successfully queued " << dataType << std::endl;
        }else{
            //Overwrite the current thing in the queue, rather than adding a new thing
            queuedDataPointers[dataType]->id = dataType;
            queuedDataPointers[dataType]->timestamp = currTimestamp;
            std::shared_ptr<byte[]> sp(new byte[dataLength]);
            queuedDataPointers[dataType]->data = sp;
            memcpy(queuedDataPointers[dataType]->data.get(), data, dataLength);
            queuedDataPointers[dataType]->dataLength = dataLength;
            // std::cout << "successfully override queued " << dataType << std::endl;
        }
        lock.unlock();
    }

    //Legacy support so I don't have to reconfigure a bunch of stuff rn (please don't use this)
    void Coms::sendData(DataTypes dataType, float data){
        byte newData[sizeof(float)];
        memcpy(newData, &data, sizeof(float));
        sendData(dataType, newData, sizeof(float));
    }
}