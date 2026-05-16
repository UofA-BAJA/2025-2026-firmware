#include "Procedure.h"
#include "DrivetrainSubsystem.h"
#include "DataStorage.h"
#include "CarLogger.h"
#include "Coms.h"

namespace BajaWildcatRacing{

    class SpeedLoggingProcedure : public Procedure {
        public:
            DrivetrainSubsystem& drivetrainSubsystem;
            DataStorage& dataStorage;
            Coms& coms;

            SpeedLoggingProcedure(DrivetrainSubsystem& drivetrainSubsystem, DataStorage& dataStorage, Coms& coms)
            : drivetrainSubsystem(drivetrainSubsystem)
            , dataStorage(dataStorage)
            , coms(coms){
                this->frequency = 30;
            }

            void init() override {
                std::cout << "Speed Logging procedure initialized!" << std::endl;
            }

            void execute() override {
                //Overall "car speed" is logged in CarStateProcedure

                //TODO: log these and send via radio
 
            }

            void end() override {
                std::cout << "Speed Logging Procedure is done for! (ended)" << std::endl;
            }

            bool isFinished() override {
                return false;
            }
    
            std::string toString() override {
                return "Speed Logging Procedure";
            }
    };
}