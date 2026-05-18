#include "Procedure.h"
#include "DrivetrainSubsystem.h"
#include "DataStorage.h"
#include "CarLogger.h"
#include "Coms.h"

namespace BajaWildcatRacing{

    class DisplacementLoggingProcedure : public Procedure {
        public:
            DrivetrainSubsystem& drivetrainSubsystem;
            DataStorage& dataStorage;
            Coms& coms;

            DisplacementLoggingProcedure(DrivetrainSubsystem& drivetrainSubsystem, DataStorage& dataStorage, Coms& coms)
            : drivetrainSubsystem(drivetrainSubsystem)
            , dataStorage(dataStorage)
            , coms(coms){
                this->frequency = 30; 
            }

            void init() override {
                std::cout << "Displacement Logging procedure initialized!" << std::endl;
            }

            void execute() override {
                ShockDisplacement disp = drivetrainSubsystem.getDisplacement();

                dataStorage.storeData(disp);
                coms.sendData(DataType::SHOCK_DISPLACEMENT, disp);
            }

            void end() override {
                std::cout << "Displacement Logging Procedure is done for! (ended)" << std::endl;
            }

            bool isFinished() override {
                return false;
            }
    
            std::string toString() override {
                return "Displacement Logging Procedure";
            }
    };
}