#include "BrakePressureSensor.h"

namespace BajaWildcatRacing
{

    BrakePressureSensor::BrakePressureSensor(CANDispatcher& canDispatcher, byte deviceId) : CANDevice(canDispatcher, deviceId) {

    }

    float BrakePressureSensor::getBrakePressure(){
        sendCanRequest(0x00, &pressure, sizeof(float));
        return pressure;
    }

}
