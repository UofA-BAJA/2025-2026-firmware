#ifndef CARCONTAINER_H
#define CARCONTAINER_H

#include "ProcedureScheduler.h"
#include "CANDispatcher.h"
#include "DataStorage.h"
#include "Coms.h"
#include "Commands.h"
#include "CarLogger.h"

#include "procedures/IMULoggingProcedure.cpp"
#include "procedures/DashProcedure.cpp"
#include "procedures/TemperatureLoggingProcedure.cpp"
#include "procedures/AccelerationProcedure.cpp"
#include "procedures/CheckEngineProcedure.cpp"
#include "procedures/CarStateProcedure.cpp"
#include "procedures/RPMLoggingProcedure.cpp"
#include "procedures/SpeedLoggingProcedure.cpp"
#include "procedures/TestProcedure.cpp"
#include "procedures/DisplacementLoggingProcedure.cpp"
#include "procedures/BrakePressureLoggingProcedure.cpp"
#include "procedures/GPSLoggingProcedure.cpp"

#include "DashSubsystem.h"
#include "IMUSubsystem.h"
#include "DrivetrainSubsystem.h"
#include "TestSubsystem.h"
#include "GPSSubsystem.h"

namespace BajaWildcatRacing
{

    class CarContainer{
        
        public:
            CarContainer(
                         ProcedureScheduler& procedureScheduler,
                         CANDispatcher& canDispatcher,
                         DataStorage& dataStorage,
                         Coms& coms
                        );
        private:
            IMUSubsystem imuSubsystem;

            DashSubsystem dashSubsystem;

            DrivetrainSubsystem drivetrainSubsystem;

            TestSubsystem testSubsystem;

            GPSSubsystem gpsSubsystem;

    };

}


#endif