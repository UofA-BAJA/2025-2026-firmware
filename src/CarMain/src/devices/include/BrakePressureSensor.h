#ifndef BRAKEPRESSURE_H
#define BRAKEPRESSURE_H

#include "CANDevice.h"

namespace BajaWildcatRacing
{

    class BrakePressureSensor : CANDevice {

        public:

            BrakePressureSensor(CANDispatcher& candispatcher, byte deviceId);

            float getBrakePressure();


        private:
            float pressure = 0.0;

    };

}

#endif