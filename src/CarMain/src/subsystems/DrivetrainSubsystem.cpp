#include "DrivetrainSubsystem.h"

namespace BajaWildcatRacing
{

    DrivetrainSubsystem::DrivetrainSubsystem(CANDispatcher& canDispatcher)
    : cvtTemperature(canDispatcher, Device::Devices::CVT_TEMP)
    , tachometer(canDispatcher, Device::Devices::TACHOMETER)
    , spedometer(canDispatcher, Device::Devices::SPEDOMETER)
    , frontBrakePressure(canDispatcher, Device::Devices::BRAKE_PRESSURE_FRONT)
    , rearBrakePressure(canDispatcher, Device::Devices::BRAKE_PRESSURE_REAR)
    {

    }

    float DrivetrainSubsystem::getCVTTemperature(){
        float temperature = cvtTemperature.getLatestTemperature();
        float tempFarenheit = (temperature * (9.0f / 5.0f)) + 32.0f;
        if(tempFarenheit > 200.0f) cvtIsHot = true;
        else cvtIsHot = false;
        return tempFarenheit;
    }

    bool DrivetrainSubsystem::isCVTHot(){
        return cvtIsHot;
    }

    EngineRPM DrivetrainSubsystem::getEngineRPM(){
        EngineRPM r;
        r.rpm = tachometer.getEngineRPM();
        return r;
    }

    WheelRPM DrivetrainSubsystem::getWheelRPM(){
        WheelRPM rpm = spedometer.getWheelRPM();
        rpm.frontLeft = (rpm.frontLeft < 0.0) ? 0.0f : rpm.frontLeft;
        rpm.frontRight = (rpm.frontRight < 0.0) ? 0.0f : rpm.frontRight;
        rpm.rear = (rpm.rear < 0.0) ? 0.0f : rpm.rear;
        return rpm;
    }

    float DrivetrainSubsystem::getCarSpeedMetersSec(){
        WheelRPM rpm = spedometer.getWheelRPM();
        return ((rpm.frontLeft + rpm.frontRight + rpm.rear) / 3.0) * 0.0289f; //Magical number
        // return spedometer.getFrontRightRPM() * 0.0289f;

    }

    float DrivetrainSubsystem::getCarSpeedMPH(){
        WheelRPM rpm = spedometer.getWheelRPM();
        return ((rpm.frontLeft + rpm.frontRight + rpm.rear) / 3.0) * 0.0647f; //Slightly different magic number
        // return spedometer.getFrontRightRPM() * 0.0647f;

    }

    BrakePressure DrivetrainSubsystem::getBrakePressure(){
        BrakePressure bp;
        bp.front = frontBrakePressure.getBrakePressure();
        bp.rear = rearBrakePressure.getBrakePressure();
        return bp;
    }

}