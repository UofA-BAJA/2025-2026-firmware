#include "Spedometer.h"

namespace BajaWildcatRacing
{
    Spedometer::Spedometer(CANDispatcher& canDispatcher, byte deviceId) : CANDevice(canDispatcher, deviceId){
        lastRPM.frontLeft = 0;
        lastRPM.frontRight = 0;
        lastRPM.rear = 0;
    }

    WheelRPM Spedometer::getWheelRPM(){
        sendCanRequest(0x00, &lastRPM, sizeof(float));
        return lastRPM;
    }


}