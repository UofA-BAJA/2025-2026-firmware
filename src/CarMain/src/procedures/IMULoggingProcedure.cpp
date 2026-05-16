#include "Procedure.h"
#include "IMUSubsystem.h"
#include "DataStorage.h"
#include "CarLogger.h"
#include "Coms.h"

#include <iomanip>

namespace BajaWildcatRacing
{

class IMULoggingProcedure : public Procedure{
    public:
        IMUSubsystem& imuSubsystem;
        DataStorage& dataStorage;
        Coms& coms;


        IMULoggingProcedure(IMUSubsystem& imuSubsystem, DataStorage& dataStorage, Coms& coms)
        : imuSubsystem(imuSubsystem)
        , dataStorage(dataStorage)
        , coms(coms)
        {

            this->frequency = 30;

        }
        
        void init() override {
            std::cout << "IMU procedure initialized!" << std::endl;
        }

        void execute() override {
            RotationXYZ rot = imuSubsystem.getRotation();
            AccelerationXYZ accel = imuSubsystem.getAcceleration();
            
            dataStorage.storeData(rot, accel);
            
            coms.sendData(DataType::IMU_ROTATION, rot);
            coms.sendData(DataType::IMU_ACCELERATION, accel);
        }

        void end() override {
            std::cout << "IMU procedure ended" << std::endl;
        }

        bool isFinished() override {
            return false;
        }

        std::string toString() override {
            return "IMU Procedure";
        }

    private:
};

}
