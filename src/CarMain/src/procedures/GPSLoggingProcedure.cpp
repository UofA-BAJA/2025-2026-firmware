#include "Procedure.h"
#include "GPSSubsystem.h"
#include "DataStorage.h"
#include "CarLogger.h"
#include "Coms.h"

namespace BajaWildcatRacing{

    class GPSLoggingProcedure : public Procedure {
        public:
            GPSSubsystem& gpsSubsystem;
            DataStorage& dataStorage;
            Coms& coms;
            bool isReady;
            bool carTimeSet;

            GPSLoggingProcedure(GPSSubsystem& gpsSubsystem, DataStorage& dataStorage, Coms& coms)
            : gpsSubsystem(gpsSubsystem)
            , dataStorage(dataStorage)
            , coms(coms){
                this->frequency = 10; 
                isReady = false;
                carTimeSet = false;
            }

            void init() override {
                isReady = false;
                carTimeSet = false;
                std::cout << "GPS Logging procedure initialized!" << std::endl;
            }

            void execute() override {
                //If not ready, check and wait
                if(!isReady){
                    isReady = gpsSubsystem.isReady();
                    if(isReady){
                        //Send commands just to start flushing the "0" from them
                        gpsSubsystem.getPosition();
                        gpsSubsystem.getUnixEpoch();
                        //Debug
                        std::cout << "GPS is ready" << std::endl;
                    }
                }
                //Normal usage
                else{
                    //Set the car time
                    if(!carTimeSet){
                        uint32_t epoch = gpsSubsystem.getUnixEpoch();
                        if(epoch > 0){
                            //TODO: set car time
                            carTimeSet = true;
                        }
                    }

                    GPSPosition pos = gpsSubsystem.getPosition();

                    dataStorage.storeData(pos); //fix me!!
                    coms.sendData(DataType::GPS_POSITION, pos);
                }
            }

            void end() override {
                std::cout << "GPS Logging Procedure is done for! (ended)" << std::endl;
            }

            bool isFinished() override {
                return false;
            }
    
            std::string toString() override {
                return "GPS Logging Procedure";
            }
    };
}