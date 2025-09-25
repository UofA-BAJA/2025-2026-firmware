#include "TestDevice.h"

namespace BajaWildcatRacing
{

    TestDevice::TestDevice(CANDispatcher& canDispatcher, byte deviceId) : CANDevice(canDispatcher, deviceId) {
        cvtTemp = 0;
    }

    TestDevice::TestStruct TestDevice::getTestStruct(){
        sendCanRequest(0x01, &test, sizeof(TestStruct));
        return test;
    }

    void test(bool lossless){
        if(lossless){
            sendLosslessCanCommand(0x02);
        }else{
            sendLossyCanCommand(0x02);
        }
    }

}