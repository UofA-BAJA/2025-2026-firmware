#include "GPSSubsystem.h"


namespace BajaWildcatRacing
{

    GPSSubsystem::GPSSubsystem(CANDispatcher& canDispatcher) : gps(canDispatcher, Device::Devices::GPS) {


    }

    GPSPosition GPSSubsystem::getPosition(){
        return gps.getPosition();
    }

    uint32_t GPSSubsystem::getUnixEpoch(){
        return gps.getUnixEpoch();
    }

    bool GPSSubsystem::isReady(){
        return gps.isReady();
    }


}