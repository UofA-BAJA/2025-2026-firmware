#include "Procedure.h"
#include "DrivetrainSubsystem.h"
#include "DataStorage.h"
#include "CarLogger.h"
#include "Coms.h"

namespace BajaWildcatRacing{

    class BrakePressureLoggingProcedure : public Procedure {
        public:
            DrivetrainSubsystem& drivetrainSubsystem;
            DataStorage& dataStorage;
            Coms& coms;

            BrakePressureLoggingProcedure(DrivetrainSubsystem& drivetrainSubsystem, DataStorage& dataStorage, Coms& coms)
            : drivetrainSubsystem(drivetrainSubsystem)
            , dataStorage(dataStorage)
            , coms(coms){
                this->frequency = 30; 
            }

            void init() override {
                std::cout << "Brake Pressure Logging procedure initialized!" << std::endl;
            }

            void execute() override {
                BrakePressure bp = drivetrainSubsystem.getBrakePressure();

                dataStorage.storeData(bp);
                coms.sendData(DataType::BRAKE_PRESSURE, bp);
            }

            void end() override {
                std::cout << "Brake Pressure Logging Procedure is done for! (ended)" << std::endl;
            }

            bool isFinished() override {
                return false;
            }
    
            std::string toString() override {
                return "Brake Pressure Logging Procedure";
            }
    };
}