#ifndef DRIVETRAINSUBSYSTEM_H
#define DRIVETRAINSUBSYSTEM_H

#include "CANDispatcher.h"
#include "CarLogger.h"
#include "DataTypes.h"

#include "Temperature.h"
#include "Tachometer.h"
#include "Spedometer.h"
#include "BrakePressureSensor.h"

namespace BajaWildcatRacing
{
    
    class DrivetrainSubsystem{
        public:

            DrivetrainSubsystem(CANDispatcher& canDispatcher);

            float getCVTTemperature();
            bool isCVTHot();

            EngineRPM getEngineRPM();
            
            WheelRPM getWheelRPM();
            float getCarSpeedMetersSec();
            float getCarSpeedMPH();

            BrakePressure getBrakePressure();

        private:
            Tachometer tachometer;

            Temperature cvtTemperature;

            Spedometer spedometer;

            BrakePressureSensor frontBrakePressure;
            BrakePressureSensor rearBrakePressure;
            
            bool cvtIsHot = false;

    };

}


#endif