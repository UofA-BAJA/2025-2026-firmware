#ifndef SPEDOMETER_H
#define SPEDOMETER_H

#include "CANDevice.h"
// #include "Devices.h"
#include "DataTypes.h"

namespace BajaWildcatRacing
{
    class Spedometer : CANDevice {
        public: 
            Spedometer(CANDispatcher& canDispatcher, byte deviceId);
            WheelRPM getWheelRPM();
        private:
            WheelRPM lastRPM;
    };
}

#endif