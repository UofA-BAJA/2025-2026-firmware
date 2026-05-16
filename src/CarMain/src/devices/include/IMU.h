#ifndef IMU_H
#define IMU_H


#include "CANDevice.h"
#include "DataTypes.h"

namespace BajaWildcatRacing
{

    class IMU : CANDevice {

        public:
            IMU(CANDispatcher& canDispatcher, byte deviceId);

            RotationXYZ getLatestRotation();

            AccelerationXYZ getLatestAcceleration();
        private:

            RotationXYZ rot;
            AccelerationXYZ accel;
    };

}



#endif