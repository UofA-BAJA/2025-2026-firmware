#ifndef DISPLACEMENT_H
#define DISPLACEMENT_H

#include "CANDevice.h"

namespace BajaWildcatRacing
{

    typedef struct ShockDisplacementPair{
        float left;
        float right;
    }ShockDisplacementPair;

    class Displacement : CANDevice {

        public:

            Displacement(CANDispatcher& candispatcher, byte deviceId);

            

            ShockDisplacementPair getDisplacement();

        private:
            ShockDisplacementPair disp;


    };

    

}

#endif