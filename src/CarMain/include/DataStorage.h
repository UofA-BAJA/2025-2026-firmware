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
            void execute(float timestamp);
            void end();

            // lol idk why this function even exists
            int getData();

            void insertCar(std::string name, int competitionYear);
            void insertSensor(std::string name, std::string manufacturer, std::string model, std::string sensor_type, std::string description);

            template<typename T>
            void storeData(T data, std::string sensor_name);

            extern template<>

        private:
            struct DataValues{
                int currentSessionID;
                double currentTimestamp;
                int dataType;
                double data;
            };

            struct DataTypeTable{
                DataType dataType;
                std::string name;
                std::string unit;
            };

            // Database handle for our sqlite database
            sqlite3* db;

            int currentSessionID = 0;
            float currentTimestamp = 0;
            int currentTimestampID = 0;

            ulong numDataInserts = 1;

            std::mutex insertBufferMutex;
            std::queue<DataValues> insertBuffer;

            // std::condition_variable insertCondition;
            // std::mutex cvMutex;

            std::thread updateDBThread;
            std::atomic<bool> running = true;

            void setupDatabase(const char* path);
            void updateDatabase();
    };

}



#endif
