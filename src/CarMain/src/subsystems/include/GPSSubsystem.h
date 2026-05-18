#ifndef GPSSUBSYSTEM_H
#define GPSSUBSYSTEM_H

#include "GPS.h"
#include "CANDispatcher.h"

namespace BajaWildcatRacing
{

    class GPSSubsystem{
        public:
            GPSSubsystem(CANDispatcher& canDispatcher);

            GPSPosition getPosition();
            uint32_t getUnixEpoch();
            bool isReady();

        private:
            GPS gps;
    };

}


#endif