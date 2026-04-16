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
        const char* insertData = "INSERT OR IGNORE INTO Data (SessionID, Timestamp, DataTypeID, Value) "
                                "VALUES (?, ?, ?, ?)";

        char* messageError;
        int exit;

        while(running.load()){
            // insertCondition.wait(conditionalLock, [this] { return insertBuffer.size() > 20; });

            std::unique_lock<std::mutex> lock(insertBufferMutex);

            std::queue<DataValues> localInsertBuffer;
            std::swap(localInsertBuffer, insertBuffer);
            lock.unlock();


            sqlite3_exec(db, "BEGIN TRANSACTION", NULL, NULL, &messageError);


            while(!localInsertBuffer.empty()){

                DataValues data = localInsertBuffer.front();
                localInsertBuffer.pop();


                int exit = 0;

                sqlite3_stmt *statement;

                exit = sqlite3_prepare_v2(db, insertData, -1, &statement, nullptr);

                if(exit){
                    std::cerr << "Can't open database: " << sqlite3_errmsg(db) << std::endl;
                    return;
                }


                // Bind values to parameters
                sqlite3_bind_int(statement, 1, data.currentSessionID);
                sqlite3_bind_double(statement, 2, data.currentTimestamp);
                sqlite3_bind_int(statement, 3, data.dataType);
                sqlite3_bind_double(statement, 4, data.data);

                // std::cout << "SessionID: " << data.currentSessionID << " DataType: " << data.dataType << " Timestamp: " << data.currentTimestamp << std::endl;

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

    void DataStorage::execute(float timestamp){

        currentTimestamp = timestamp;
    }


    int i = 0;

    int DataStorage::getData(){
        std::cout << "Get Data Called, i: " << i << std::endl;
        return i;
    }


    /*
    *  Method:  storeData
    *
    *  Purpose: Inserts a given floating point value to store in association with a data
    *           type and timestamp. 
    *
    *  Pre-Condition:  [Any non-obvious conditions that must exist
    *              or be true before we can expect this method to
    *              function correctly.]
    *
    *  Post-Condition: The data given will be stored in the database in the data table;
    *                  The data is stored in association with the given data type and the
    *                  current car timestamp (time since car program started);
    *
    *  Parameters:
    *          data -- the floating point data value to enter into the database
    *          dataType -- the type of data to enter. ie. IMU X Rotation
    *
    *  Returns: None
    *
    */

    void DataStorage::storeData(float data, DataType dataType){

        DataValues dataToStore = {};
        dataToStore.currentSessionID = currentSessionID;
        dataToStore.currentTimestamp = currentTimestamp;
        dataToStore.dataType = dataType;
        dataToStore.data = data;


        numDataInserts++;
        std::lock_guard<std::mutex> lock (insertBufferMutex);
        insertBuffer.push(dataToStore);
    }


    void DataStorage::setupDatabase(const char* path){

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
        
        char* errMsg = nullptr;
        int rc = sqlite3_exec(db, database_schema, nullptr, nullptr, &errMsg);
        if(rc != SQLITE_OK){
            std::cerr << "SQL error: " << errMsg << std::endl;
            sqlite3_free(errMsg);
        }

    }

}
