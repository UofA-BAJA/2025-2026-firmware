#ifndef DRIVETRAINSUBSYSTEM_H
#define DRIVETRAINSUBSYSTEM_H

#include "CANDispatcher.h"
#include "CarLogger.h"
#include "DataTypes.h"

#include "Temperature.h"
#include "Tachometer.h"
#include "Spedometer.h"
#include "BrakePressureSensor.h"
#include "Displacement.h"

namespace BajaWildcatRacing
{
    
    class DrivetrainSubsystem{
        public:

            DrivetrainSubsystem(CANDispatcher& canDispatcher);

            CVTTemp getCVTTemperature();
            bool isCVTHot();

            EngineRPM getEngineRPM();
            
            WheelRPM getWheelRPM();
            float getCarSpeedMetersSec();
            float getCarSpeedMPH();

            BrakePressure getBrakePressure();

            ShockDisplacement getDisplacement();

        private:
            Tachometer tachometer;

            Temperature cvtTemperature;

            Spedometer spedometer;

            BrakePressureSensor frontBrakePressure;
            BrakePressureSensor rearBrakePressure;

            Displacement frontDisplacement;
            Displacement rearDisplacement;
            
            bool cvtIsHot = false;

    };

}


#endif