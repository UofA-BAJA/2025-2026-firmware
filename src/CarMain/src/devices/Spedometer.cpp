#include "Spedometer.h"

namespace BajaWildcatRacing
{
    Spedometer::Spedometer(CANDispatcher& canDispatcher) : CANDevice(canDispatcher){

    }

    float Spedometer::getFrontLeftRPM(){
        sendCanRequest(Device::SPEDOMETER, 0x02, &frontLeftRPM);
        return frontLeftRPM;
    }

    float Spedometer::getFrontRightRPM(){
        sendCanRequest(Device::SPEDOMETER, 0x03, &frontRightRPM);
        return frontRightRPM;
    }

    float Spedometer::getRearRPM(){
        sendCanRequest(Device::SPEDOMETER, 0x01, &rearRPM);
        return rearRPM;
    }

}