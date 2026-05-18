#ifndef DASH_H
#define DASH_H

#include "CANDevice.h"
#include "Devices.h"
 
namespace BajaWildcatRacing
{

    class Dash : CANDevice
    {
        public:
            Dash(CANDispatcher& canDispatcher, byte deviceId);
            void sendRPM(float rpm);
            void sendSpeed(float speed);
            void sendCVTTemp(float cvtTemp);
            void sendTimeSeconds(float seconds);
            void sendDistance(float distance);
            //void sendFaults(int errorCode);

        private:

    };

}


#endif