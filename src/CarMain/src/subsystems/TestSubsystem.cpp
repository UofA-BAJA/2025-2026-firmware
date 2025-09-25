#include "TestSubsystem.h"

namespace BajaWildcatRacing{

    TestSubsystem::TestSubsystem(CANDispatcher& canDispatcher) : dev(canDispatcher, 0x1F){
        
    }

    TestDevice::TestStruct TestSubsystem::getTestStruct(){
        return dev.getTestStruct();
    }

    void test(bool lossless){
        dev.test(lossless);
    }

}