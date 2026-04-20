/* 
 *
 *
 * Author: Matthew Larson
 */

#ifndef DATASTORAGE_H
#define DATASTORAGE_H

#include <sqlite3.h>
#include <iostream>
#include <string.h>
#include <queue>

#include <mutex>
#include <thread>
#include <atomic>
#include <fstream>
#include <sstream>
#include <chrono>
#include <iomanip>

#include "SqlCommands.h"
#include "DataTypes.h"

namespace BajaWildcatRacing
{

    class DataStorage {
        public:
            DataStorage(const char* path);
            void end();

            // lol idk why this function even exists
            int getData();

            void insertCar(std::string name, int competitionYear);
            void insertSensor(std::string name, std::string manufacturer, std::string model, std::string sensorType, std::string description);

            void storeData(ShockDisplacement data);
            void storeData(RotationXYZ rotData, AccelerationXYZ accData);
            void storeData(BrakePressure data);
            void storeData(WheelSpeed data);
            void storeData(BrakePressure data);


        private:
            // Database handle for our sqlite database
            sqlite3* db;

            ulong numDataInserts = 1;

            std::mutex insertBufferMutex;
            std::queue<sqlite3_stmt *> insertBuffer;

            std::thread updateDBThread;
            std::atomic<bool> running = true;


            void queueSqlStatement(sqlite3_stmt *statement);

            void setupDatabase(const char* path);
            void updateDatabase();
    };

}



#endif
