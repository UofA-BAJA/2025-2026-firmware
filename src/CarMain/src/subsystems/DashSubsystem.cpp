#include "DashSubsystem.h"

namespace BajaWildcatRacing
{

    DashSubsystem::DashSubsystem(CANDispatcher& canDispatcher) : dash(canDispatcher, Device::Devices::DASH){

    }


    void DashSubsystem::sendSpeed(float speed){
        dash.sendSpeed(speed);
    }

    void DashSubsystem::sendRPM(float rpm){
        dash.sendRPM(rpm);
    }

    void DashSubsystem::sendCVTTemp(float cvtTemp){
        dash.sendCVTTemp(cvtTemp);
    }

    void DashSubsystem::sendTimeSeconds(float seconds){
        dash.sendTimeSeconds(seconds);
    }

    void DashSubsystem::sendDistance(float distance){
        dash.sendDistance(distance);
    }

}
