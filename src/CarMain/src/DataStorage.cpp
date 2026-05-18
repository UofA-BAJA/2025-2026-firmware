#include "DataStorage.h"

namespace BajaWildcatRacing
{

    DataStorage::DataStorage(const char* path)
    {
        setupDatabase(path);

        updateDBThread = std::thread(&DataStorage::updateDatabase, this);

        std::cout << "Data Storage initialized" << std::endl;
    }


    void DataStorage::updateDatabase(){

        char* messageError;
        int exit;

        while(running.load()){

            std::unique_lock<std::mutex> lock(insertBufferMutex);

            std::queue<sqlite3_stmt *> localInsertBuffer;
            std::swap(localInsertBuffer, insertBuffer);
            lock.unlock();


            sqlite3_exec(db, "BEGIN TRANSACTION", NULL, NULL, &messageError);


            while(!localInsertBuffer.empty()){

                sqlite3_stmt *statement = localInsertBuffer.front();
                localInsertBuffer.pop();

                int exit = 0;

                exit = sqlite3_step(statement);

                if(exit != SQLITE_DONE){
                    std::cerr << "Execution failed: " << sqlite3_errmsg(db) << std::endl;
                }
                sqlite3_finalize(statement);
            }

            sqlite3_exec(db, "END TRANSACTION", NULL, NULL, &messageError);

            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }



    void DataStorage::end(){
        running = false;

        if (updateDBThread.joinable()) {
            updateDBThread.join();  // Wait for the thread to finish
        }

        sqlite3_close(db);

    }


    int DataStorage::getData(){
        std::cout << "Shame on you for calling getData()" << std::endl;
        return 1;       // Always error
    }


    // statement must be a prepared statement!!!!
    // It can not be null!
    void DataStorage::queueSqlStatement(sqlite3_stmt *statement){

        if(statement == nullptr){
            // literally the end of the world
            return;
        }

        numDataInserts++;

        std::lock_guard<std::mutex> lock (insertBufferMutex);
        insertBuffer.push(statement);
    }

    
    void DataStorage::setupDatabase(const char* path){


        // Create new .db file with the current time in the name

        auto now = std::chrono::system_clock::now();
        // Format: Year-Month-Day_Hour-Minute-Second
        std::time_t t = std::chrono::system_clock::to_time_t(now);

        std::stringstream ss;
        ss << std::put_time(std::localtime(&t), "%Y-%m-%d_%H-%M-%S");
        std::string timestamp = ss.str();
        
        std::string fullPath = std::string(path) + "/baja_data_" + timestamp +".db";

        int exit = 0;
        // If database does not already exist, it will be created.
        exit = sqlite3_open(fullPath.c_str(), &db);

        if (exit) { 
            std::cerr << "Error open DB: " << sqlite3_errmsg(db) << std::endl; 
            return;
        } 
        else
        {
            // std::cout << "Opened Database Successfully!" << std::endl; 
        }
        

        // Execute database schema
        char* errMsg = nullptr;
        int rc = sqlite3_exec(db, DATABASE_SCHEMA, nullptr, nullptr, &errMsg);
        if(rc != SQLITE_OK){
            std::cerr << "SQL error: " << errMsg << std::endl;
            sqlite3_free(errMsg);
        }
    }

    
    void DataStorage::storeData(ShockDisplacement data){

        storeLinearActuator("LA_FR", 5, data.frontRight);
        storeLinearActuator("LA_FL", 5, data.frontLeft);
        storeLinearActuator("LA_RR", 5, data.rearRight);
        storeLinearActuator("LA_RL", 5, data.rearLeft);
    }

    void DataStorage::storeLinearActuator(const char* sensor, int sensor_len, float displacement){
        sqlite3_stmt *statement;
        
        int exit = sqlite3_prepare_v2(db, INSERT_LINEAR_ACTUATOR, -1, &statement, nullptr);
        
        if(exit){
            std::cerr << "Can't open database: " << sqlite3_errmsg(db) << std::endl;
            return;
        }
        // Bind values to parameters
        sqlite3_bind_int64(statement, 1, CarTime::getUnixEpoch());      // TODO: Need unix epoch somehow
        sqlite3_bind_text(statement, 2, VEHICLE_NAME, strlen(VEHICLE_NAME), NULL);
        sqlite3_bind_text(statement, 3, sensor, sensor_len, NULL);
        sqlite3_bind_double(statement, 4, displacement);


        queueSqlStatement(statement);

    }
    
    void DataStorage::storeData(RotationXYZ rotData, AccelerationXYZ accData){
        
        sqlite3_stmt *statement;
        
        int exit = sqlite3_prepare_v2(db, INSERT_IMU, -1, &statement, nullptr);
        
        if(exit){
            std::cerr << "Can't open database: " << sqlite3_errmsg(db) << std::endl;
            return;
        }
        // Bind values to parameters
        sqlite3_bind_int64(statement, 1, CarTime::getUnixEpoch());      // TODO: Need unix epoch somehow
        sqlite3_bind_text(statement, 2, VEHICLE_NAME, strlen(VEHICLE_NAME), NULL);
        sqlite3_bind_text(statement, 3, "IMU", 3, NULL);
        sqlite3_bind_double(statement, 4, accData.accelerationx);
        sqlite3_bind_double(statement, 5, accData.accelerationy);
        sqlite3_bind_double(statement, 6, accData.accelerationz);
        sqlite3_bind_double(statement, 7, rotData.rotationx);
        sqlite3_bind_double(statement, 8, rotData.rotationy);
        sqlite3_bind_double(statement, 9, rotData.rotationz);


        queueSqlStatement(statement);
    }

    void DataStorage::storeData(BrakePressure data){

        storeBrakePressure("BP_F", 4, data.front);
        storeBrakePressure("BP_R", 4, data.rear);
    }

    void DataStorage::storeBrakePressure(const char* sensor, int sensor_len, float psi){
        sqlite3_stmt *statement;
        
        int exit = sqlite3_prepare_v2(db, INSERT_PRESSURE, -1, &statement, nullptr);
        
        if(exit){
            std::cerr << "Can't open database: " << sqlite3_errmsg(db) << std::endl;
            return;
        }

        // Bind values to parameters
        sqlite3_bind_int64(statement, 1, CarTime::getUnixEpoch());      // TODO: Need unix epoch somehow
        sqlite3_bind_text(statement, 2, VEHICLE_NAME, strlen(VEHICLE_NAME), NULL);
        sqlite3_bind_text(statement, 3, sensor, sensor_len, NULL);
        sqlite3_bind_double(statement, 4, psi);

        queueSqlStatement(statement);
    }

    void DataStorage::storeData(WheelSpeed data){
        storeTachometer("Sped_FR", 7, data.frontRight);
        storeTachometer("Sped_FL", 7, data.frontLeft);
        storeTachometer("Sped_R", 6, data.rear);
    }

    void DataStorage::storeData(EngineRPM data){
        storeTachometer("Engine_Tach", 11, data.rpm);
    }

    void DataStorage::storeTachometer(const char* sensor, int sensor_len, float rpm){
        
        sqlite3_stmt *statement;
        
        int exit = sqlite3_prepare_v2(db, INSERT_TACHOMETER, -1, &statement, nullptr);
        
        if(exit){
            std::cerr << "Can't open database: " << sqlite3_errmsg(db) << std::endl;
            return;
        }

        // Bind values to parameters
        sqlite3_bind_int64(statement, 1, CarTime::getUnixEpoch());      // TODO: Need unix epoch somehow
        sqlite3_bind_text(statement, 2, VEHICLE_NAME, strlen(VEHICLE_NAME), NULL);
        sqlite3_bind_text(statement, 3, sensor, sensor_len, NULL);
        sqlite3_bind_double(statement, 4, rpm);

        queueSqlStatement(statement);
    }

    void DataStorage::storeData(CVTTemp data){
        sqlite3_stmt *statement;
        
        int exit = sqlite3_prepare_v2(db, INSERT_TEMPERATURE, -1, &statement, nullptr);
        
        if(exit){
            std::cerr << "Can't open database: " << sqlite3_errmsg(db) << std::endl;
            return;
        }
        // Bind values to parameters
        sqlite3_bind_int64(statement, 1, CarTime::getUnixEpoch());      // TODO: Need unix epoch somehow
        sqlite3_bind_text(statement, 2, VEHICLE_NAME, strlen(VEHICLE_NAME), NULL);
        sqlite3_bind_text(statement, 3, "CVT_Temp", 8, NULL);
        sqlite3_bind_double(statement, 4, data.temp);

        queueSqlStatement(statement);
    }

    void DataStorage::storeData(GPSPosition data){
        sqlite3_stmt *statement;
        
        int exit = sqlite3_prepare_v2(db, INSERT_GPS, -1, &statement, nullptr);
        
        if(exit){
            std::cerr << "Can't open database: " << sqlite3_errmsg(db) << std::endl;
            return;
        }
        // Bind values to parameters
        sqlite3_bind_int64(statement, 1, CarTime::getUnixEpoch());      // TODO: Need unix epoch somehow
        sqlite3_bind_text(statement, 2, VEHICLE_NAME, strlen(VEHICLE_NAME), NULL);
        sqlite3_bind_text(statement, 3, "GPS", 3, NULL);
        sqlite3_bind_double(statement, 4, data.latitude);
        sqlite3_bind_double(statement, 4, data.longitude);
        sqlite3_bind_double(statement, 4, data.atltitude);

        queueSqlStatement(statement);
    }

}