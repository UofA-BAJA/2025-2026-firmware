#include "GPS.h"

namespace BajaWildcatRacing
{

    GPS::GPS(CANDispatcher& canDispatcher, byte deviceId) : CANDevice(canDispatcher, deviceId){
        pos.latitude = 0.0;
        pos.longitude = 0.0;
        pos.atltitude = 0.0;
    }

    GPSPosition GPS::getPosition(){
        sendCanRequest(0x00, &pos, sizeof(GPSPosition));
        return pos;
    }

    uint32_t GPS::getUnixEpoch(){
        sendCanRequest(0x01, &unixEpoch, sizeof(GPSPosition));
        return unixEpoch;
    }

    bool GPS::isReady(){
        sendCanRequest(0x02, &isGPSReady, sizeof(GPSPosition));
        return isGPSReady;
    }

}
