#ifndef GPS_H
#define GPS_H


#include "CANDevice.h"
#include "DataTypes.h"

namespace BajaWildcatRacing
{

    class GPS : CANDevice {

        public:
            GPS(CANDispatcher& canDispatcher, byte deviceId);

            GPSPosition getPosition();
            uint32_t getUnixEpoch();
            bool isReady();
            
        private:
            GPSPosition pos;
            uint32_t unixEpoch = 0;
            bool isGPSReady = false;

    };

}



#endif