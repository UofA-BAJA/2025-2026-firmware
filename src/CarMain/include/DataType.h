#ifndef DATATYPE_H
#define DATATYPE_H

namespace BajaWildcatRacing
{

    // Define structs that are to be sent over the CAN bus.
    // These structs need to be synced across CAN devices and the main 
    // car computer.

    struct imu
    {
        float ax;
        float ay;
        float az;
        // other stuff idk
    };

    struct temperature
    {
        float temp;
    };

    struct tachometer
    {
        float rpm;

    };

}

#endif