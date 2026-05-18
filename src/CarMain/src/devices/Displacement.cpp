#include "Displacement.h"

namespace BajaWildcatRacing
{

    Displacement::Displacement(CANDispatcher& canDispatcher, byte deviceId) : CANDevice(canDispatcher, deviceId) {
        disp.left = 0;
        disp.right = 0;
    }

    ShockDisplacementPair Displacement::getDisplacement(){
        sendCanRequest(0x00, &disp, sizeof(ShockDisplacementPair));
        return disp;
    }
    
}
