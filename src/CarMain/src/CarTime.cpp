#include "CarTime.h"

namespace BajaWildcatRacing
{

    float CarTime::currTimeSeconds = 0.0f;

    float CarTime::getElapsedTimeSeconds(){
        return CarTime::currTimeSeconds;
    }

    int CarTime::getUnixEpoch(){

        return 0;
    }

    void CarTime::setElapsedTimeSeconds(float time){
        CarTime::elapsedTimeSeconds = time;
    }

}
