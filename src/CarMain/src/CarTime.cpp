#include "CarTime.h"

namespace BajaWildcatRacing
{

    float CarTime::elapsedTimeSeconds = 0.0f;
    std::int64_t CarTime::currUnixEpoch = 0;

    float CarTime::getElapsedTimeSeconds(){
        return CarTime::elapsedTimeSeconds;
    }

    int64_t CarTime::getUnixEpoch(){

        return currUnixEpoch;
    }

    void CarTime::setElapsedTimeSeconds(float time){
        CarTime::elapsedTimeSeconds = time;
    }

    void CarTime::setUnixEpoch(std::int64_t epoch){
        CarTime::currUnixEpoch = epoch;
    }

}
