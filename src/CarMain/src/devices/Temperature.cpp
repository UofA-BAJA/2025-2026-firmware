#include "Temperature.h"

namespace BajaWildcatRacing
{

    Temperature::Temperature(CANDispatcher& canDispatcher) : CANDevice(canDispatcher){

    }

    float Temperature::getLatestTemperature(){

        sendCanRequest(Device::CVT_TEMP, 0x01, &temperature);
        return temperature;

    }

}
