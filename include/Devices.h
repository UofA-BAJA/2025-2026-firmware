#ifndef DEVICES_H
#define DEVICES_H

namespace Device
{

enum Devices
{
    IMU = 0x01,
    CVT_TEMP = 0x02,
    SPEDOMETER = 0x03,
    TACHOMETER = 0x04,
    DISPLACEMENT_FRONT = 0x05,
    DISPLACEMENT_REAR = 0x06,
    BRAKE_PRESSURE_FRONT = 0x07,
    BRAKE_PRESSURE_REAR = 0x08,
    DASH = 0x09,
    GPS = 0xA
};

}


#endif