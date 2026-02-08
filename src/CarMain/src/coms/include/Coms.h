#ifndef COMS_H
#define COMS_H

#include "Commands.h"
#include "ProcedureScheduler.h"
#include "CarLogger.h"

#include "Commands.h"
#include "Constants.h"
#include "DataTypes.h"

#include <iostream>

// For multithreading
#include <mutex>
#include <thread>
#include <atomic>

#include <chrono> // I believe this is used for the sleep function in the executeRadio loop
#include <bitset> // No idea what this is for, but I think it's important for something
#include <unordered_set>
#include <memory>

// Radio library
#include <RH_RF95.h>


namespace BajaWildcatRacing
{

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

            void sendData(DataTypes dataType, float data);


        private:

            // This radio should only be accessed from the radioThread
            // We do not want a mutex to protect it, as it would bee very slow
            // and defeat the purpose of multithreading
            RH_RF95 rf95;

            
            
            const bool RADIO_ACTIVE = true;

            PitCommandState currentPitCommandState = PitCommandState::IDLE;

            float currTimestamp = 0;
            void executeRadio();
            void radioTransmit();

            void transmitLiveData();
            void recieveCommand();
            void idle();

            void tryUpdateState();


            ProcedureScheduler& procedureScheduler;

            int liveStreamCount = 0;

            //Mutexes for multithreaded safety
            std::mutex timestampMutex;
            std::mutex procedureSchedulerMutex;
            std::mutex dataQueueMutex;

            //Actual 2nd thread
            std::thread radioThread;
            std::atomic<bool> running = RADIO_ACTIVE;

    };

}


#endif
