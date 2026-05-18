#include "Procedure.h"
#include "DrivetrainSubsystem.h"
#include "DataStorage.h"
#include "CarLogger.h"
#include "Coms.h"

#include <iomanip>

namespace BajaWildcatRacing
{

class TemperatureLoggingProcedure : public Procedure{
    public:
        DrivetrainSubsystem& drivetrainSubsystem;
        DataStorage& dataStorage;
        Coms& coms;


        TemperatureLoggingProcedure(DrivetrainSubsystem& drivetrainSubsystem, DataStorage& dataStorage, Coms& coms)
        : drivetrainSubsystem(drivetrainSubsystem)
        , dataStorage(dataStorage)
        , coms(coms)
        {


            this->frequency = 30; 

        }
        
        void init() override {
            std::cout << "Temperature procedure initialized!" << std::endl;
        }

        void execute() override {
            CVTTemp cvt_temperature = drivetrainSubsystem.getCVTTemperature();

            dataStorage.storeData(cvt_temperature);
            coms.sendData(DataType::CVT_TEMPERATURE, cvt_temperature);
        }

        void end() override {
            std::cout << "Temperature procedure ended" << std::endl;
        }

        bool isFinished() override {
            return false;
        }

        std::string toString() override {
            return "Temperature Procedure";
        }

    private:
};

}
