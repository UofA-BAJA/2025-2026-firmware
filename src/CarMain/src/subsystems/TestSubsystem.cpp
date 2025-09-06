#include "TestSubsystem.h"

namespace BajaWildcatRacing{

    TestSubsystem::TestSubsystem(CANDispatcher& canDispatcher) : dev(canDispatcher, 0x69){

    }

    TestDevice::TestStruct TestSubsystem::getTestStruct(){
        return dev.getTestStruct();
    }

}