#include "TestDevice.h"

namespace BajaWildcatRacing
{

    TestDevice::TestDevice(CANDispatcher& canDispatcher, byte deviceId) : CANDevice(canDispatcher, deviceId) {

    }

    TestDevice::TestStruct TestDevice::getTestStruct(){
        sendCanRequest(0x01, &test, sizeof(TestStruct));
        return test;
    }

}