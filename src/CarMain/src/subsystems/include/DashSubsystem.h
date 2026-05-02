#ifndef DASHSUBSYSTEM_H
#define DASHSUBSYSTEM_H

#include "Dash.h"
#include "CANDispatcher.h"

namespace BajaWildcatRacing
{

    class DashSubsystem {
        public:
            DashSubsystem(CANDispatcher& canDispatcher);
            void sendSpeed(float speed);
            void sendRPM(float rpm);
            void sendCVTTemp(float cvtTemp);
            void sendTimeSeconds(float seconds);
            void sendDistance(float distance);

        private:
            Dash dash;
    };
    
}



#endif