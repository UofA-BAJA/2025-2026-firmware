#ifndef IMUSUBSYSTEM_H
#define IMUSUBSYSTEM_H

#include "IMU.h"
#include "CANDispatcher.h"

namespace BajaWildcatRacing
{

    class IMUSubsystem{
        public:
            IMUSubsystem(CANDispatcher& canDispatcher);
            RotationXYZ getRotation();
            AccelerationXYZ getAcceleration();

        private:
            IMU imu;
    };

}


#endif