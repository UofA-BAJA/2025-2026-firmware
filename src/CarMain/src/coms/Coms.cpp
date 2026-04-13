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
        running = false;
        std::cout << "Coms are stopping..." << std::endl;
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
        rf95.setModemConfig(RH_RF95::Bw500Cr45Sf128); //Selects maximum bitrate 

        //TODO: define what the addresses we are using are
        // rf95.setThisAddress(1);
        // rf95.setHeaderFrom(1);
        // rf95.setHeaderTo(2);
    
        std::cout << "RF95 initialized!!!" << std::endl;

        int cycleTimeNs = (1.0 / currentFrequency) * 1000000000L;
        std::cout << cycleTimeNs << std::endl;
        std::chrono::steady_clock::time_point startTime;
        std::chrono::steady_clock::time_point endTime;

        std::chrono::steady_clock::time_point lastRxTime = std::chrono::steady_clock::now();

        while(running){
            startTime = std::chrono::steady_clock::now();

            //Run correct function depending on state
            if(currentPitCommandState == PitCommandState::LIVE_DATA_TRANSMIT){
                transmitLiveData();
            }else if(currentPitCommandState == PitCommandState::WAIT_COMMAND_RECIEVE){
                recieveCommand();
                switchToRX = false;
                currentPitCommandState = PitCommandState::LIVE_DATA_TRANSMIT;
            }else if(currentPitCommandState == PitCommandState::IDLE){
                idle();
            }
            


            //Only wait for the amount of time we have left so we have a somewhat steady frequency
            endTime = std::chrono::steady_clock::now();
            int64_t timeTaken = std::chrono::duration_cast<std::chrono::nanoseconds>(endTime - startTime).count(); 
            // std::cout << timeTaken << std::endl;
            if(timeTaken < cycleTimeNs){
                std::this_thread::sleep_for(std::chrono::nanoseconds(cycleTimeNs - timeTaken));
                overloaded = false;
                // std::cout << "ok" << std::endl;
            }else{
                std::cout << "Radio is overloaded!! Please reduce datatypes or frequency." << std::endl;
                overloaded = true;
            }
            
            //Determine if it's time to do the RX switch
            int64_t switchInterval = std::chrono::duration_cast<std::chrono::nanoseconds>(endTime - lastRxTime).count(); 
            if(switchInterval > RX_SWITCH_INTERVAL){
                std::cout << "switching to RX on next cycle" << std::endl;
                switchToRX = true;
                lastRxTime = endTime;
            }
            
        }
        std::cout << "Coms are stopped." << std::endl;
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
        byte sentDataLength = 0;
        int dataStartPos = 0;

        while(!localQueue.empty()){
            std::shared_ptr<DataFrame> nextFrame = localQueue.front();
            localQueue.pop();
            // std::cout << "Sending frame with id " << nextFrame->id << std::endl;

            int nextSize = 2 + sizeof(float) + nextFrame->dataLength;

            //If the next data is about to go past over the max length, send what we have out
            if(sentDataLength + nextSize > RH_RF95_MAX_MESSAGE_LEN){
                std::cout << "Sending data now! Not done this cycle. Size: " << sentDataLength << std::endl;
                radioTransmit(data, sentDataLength, false);
                sentDataLength = 0;
                dataStartPos = 0;
            }

            //Add ID (1 byte), timestamp (float) and actual data to data to send
            sentDataLength += nextSize;

            //Copy ID, data length, timestamp, data
            memcpy(data + dataStartPos, &nextFrame->id, 1);
            dataStartPos += 1;
            memcpy(data + dataStartPos, &nextFrame->timestamp, sizeof(float));
            dataStartPos += sizeof(float);
            memcpy(data + dataStartPos, &nextFrame->dataLength, 1);
            dataStartPos += 1;
            memcpy(data + dataStartPos, nextFrame->data.get(), nextFrame->dataLength);
            dataStartPos += nextFrame->dataLength;
        }

        //Send final packet if there's actually data there
        if(sentDataLength > 0){
            // std::cout << "Sending data now! Size: " << sentDataLength << std::endl;
            if(switchToRX){
                currentPitCommandState = PitCommandState::WAIT_COMMAND_RECIEVE;
                rxSuccessful = false;
            }

            radioTransmit(data, sentDataLength, switchToRX);
        }
    }

    void Coms::radioTransmit(byte data[], byte sentDataLength, bool rxSwitchFlag){
        //If the mode isn't in idle mode (edge case check)
        if(rf95.mode() != RH_RF95::RHModeIdle){

            //Wait up to 500 ms for it to be done
            // uint8_t irqFlags = rf95.spiRead(RH_RF95_REG_12_IRQ_FLAGS);
            int repeats = 0;
            // while (!(irqFlags & RH_RF95_TX_DONE) && repeats < 5){
            while ((rf95.mode() != RH_RF95::RHModeIdle) && repeats < 500){
                // std::cout << "repeat " << repeats << std::endl;
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
                repeats++;
                // irqFlags = rf95.spiRead(RH_RF95_REG_12_IRQ_FLAGS);
                
            }

            //Still not done??? Timeout
            if(repeats >= 500){
                std::cout << "TIMEOUT Radio TX" << std::endl; 
                return;
            }
            // else{
            //     //Clear the flags
            //     rf95.spiWrite(RH_RF95_REG_12_IRQ_FLAGS, RH_RF95_TX_DONE);
            //     // std::this_thread::sleep_for(std::chrono::milliseconds(5));
            //     // std::cout << "Cleared flags" << std::endl;

            //     //Set the mode to idle
            //     rf95.setModeIdle();
            //     //Wait a brief amount of time for the mode to change
            //     std::this_thread::sleep_for(std::chrono::milliseconds(10));
            // }
        }


        //Set flags appropriately 
        uint8_t switchFlag = (1 & rxSwitchFlag);
        uint8_t rxSuccessFlag = (1 & rxSuccessful) << 1;
        uint8_t overloadFlag = (1 & overloaded) << 2;
        rf95.setHeaderFlags(rxSwitchFlag | rxSuccessful | overloaded);

        // std::cout << "about to send the data" << std::endl;
        if(!rf95.send(data, sentDataLength)){
            //Failed to send
            std::cout << "Failed to send data frame." << std::endl;
        }
    }

    void Coms::recieveCommand(){
        //TODO: need to make it so it doesn't throw an overload error when doing this
        
        
        int repeats = 0;
        while(!rf95.available() && repeats < 500){
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
            repeats++;
        }

        if(repeats < 500){
            uint8_t rxBuffer[RH_RF95_MAX_MESSAGE_LEN]; 
            uint8_t rxLength = sizeof(rxBuffer);
            if(rf95.recv(rxBuffer, &rxLength)){
                std::bitset<8> x(rxLength);
                std::cout << "rx length: " << x << std::endl;
                if(rxLength == 0){
                    //Zero length: no commands, go back to live data transmit
                    std::cout << "zero commands recieved" << std::endl;
                    rxSuccessful = true;
                    return;
                }else{
                    //Actual command recieved
                    std::cout << "else" << std::endl;
                    for(int i = 0; i < rxLength; i++){
                        std::cout << "reading rx byte: " << i << std::endl;
                        if(rxBuffer[i] == 0){
                            //Change frequency 
                            //TODO: how are we encoding this lmao
                            std::cout << "recieved frequency command: " << std::endl;
                        }else if(rxBuffer[i] == 1){
                            //Change commands
                            i++;
                            if(rxLength - i < 32){
                                //Not enough i's left 
                                std::cerr << "Error: recieved too short of a frame for the data change command" << std::endl;
                                rf95.setModeIdle();
                                return;
                            }
                        
                            memcpy(dataTypeMask, rxBuffer, 32);
                            i += 31; //don't want to skip over the end, only skip 31 places
                        }else{
                            //Non-special command
                            // i++;
                            std::cout << "proc scheduler command recieved" << std::endl;
                            procedureScheduler.receiveComCommand((Command)rxBuffer[i]);
                        }
                    }
                    rf95.setModeIdle();
                    rxSuccessful = true;
                }
            }else{
                std::cout << "ERROR: Radio Recieve failure" << std::endl;
            }
        }else{
            std::cout << "no command recieved" << std::endl;
            rf95.setModeIdle();
            currentPitCommandState = PitCommandState::LIVE_DATA_TRANSMIT;
        }

        
    }

    void Coms::idle(){
        //listen for commands
        //this is different to setting running = false 
    }

    void Coms::sendData(DataType dataType, byte data[], byte dataLength){
        //Only send if less than 256
        if(dataType >= 256){
            //Print for debugging, should be resolved before actual use
            std::cout << "Error: Cannot send datatype >= 256 over radio! (attempted to send " << dataType << ")" << std::endl;
            return;
        }

        if(dataLength >= RH_RF95_MAX_MESSAGE_LEN - 2 - sizeof(float)){
            //Print for debugging, should be resolved before actual use
            std::cout << "Error: Cannot send data with length >= 246 over radio! (attempted to send data length " << dataLength << ", datatype " << dataType << ")" << std::endl;
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

    template<typename T>
    void Coms::sendData(DataType dataType, T data){
        byte newData[sizeof(T)];
        memcpy(newData, &data, sizeof(T));
        sendData(dataType, newData, sizeof(T));
    }
}