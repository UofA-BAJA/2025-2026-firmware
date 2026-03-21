#ifndef COMS_H
#define COMS_H

//Baja Includes
#include "Commands.h"
#include "ProcedureScheduler.h"
#include "CarLogger.h"

#include "Commands.h"
#include "Constants.h"
#include "DataTypes.h"

//Standard Includes
#include <iostream>
#include <queue>

// For multithreading
#include <mutex>
#include <thread>
#include <atomic>

#include <chrono> // I believe this is used for the sleep function in the executeRadio loop
#include <bitset> // No idea what this is for, but I think it's important for something
#include <memory> // unique_ptr, etc


// Radio library
#include <RH_RF95.h>


namespace BajaWildcatRacing
{
    using byte = unsigned char;

    class Coms{

        enum PitCommandState{
            IDLE,
            LIVE_DATA_TRANSMIT,
            WAIT_COMMAND_RECIEVE
        };


        public:
            Coms(ProcedureScheduler& procedureScheduler);


            void execute(float timestamp);
            void end();

            void sendData(DataTypes dataType, byte data[], int dataLength);
            void sendData(DataTypes dataType, float data); //Legacy overload

        private:

            // This radio should only be accessed from the radioThread
            // We do not want a mutex to protect it, as it would bee very slow
            // and defeat the purpose of multithreading
            RH_RF95 rf95;

            
            
            const bool RADIO_ACTIVE = true;

            PitCommandState currentPitCommandState = PitCommandState::LIVE_DATA_TRANSMIT;

            std::atomic<float> currTimestamp = 0;
            void executeRadio();
            void radioTransmit();

            void transmitLiveData();
            void recieveCommand();
            void idle();

        


            ProcedureScheduler& procedureScheduler;

            typedef struct DataFrame_s{
                byte id;
                float timestamp;
                std::shared_ptr<byte[]> data;
                int dataLength;
            } DataFrame;

            //Data Queueing Stuff
            byte dataTypeMask[32]; //"Should we send this datatype??""
            std::shared_ptr<DataFrame> queuedDataPointers[256]; //A way to make sure we have the latest data in the queue (and a pointer to update it)
            std::queue<std::shared_ptr<DataFrame>> queuedData; //The queue
            

            //Mutex for multithreaded safety
            std::mutex dataQueueMutex;

            //Actual 2nd thread
            std::thread radioThread;
            bool running = RADIO_ACTIVE;

            //RX Command stuff
            bool rxSuccessful = false;

    };

}


#endif
