#ifndef CARTIME_H
#define CARTIME_H

#include <iostream>
#include <cstdint>

namespace BajaWildcatRacing
{

    class CarTime{

        public:

            static float getElapsedTimeSeconds();
            static std::int64_t getUnixEpoch();
            static void setElapsedTimeSeconds(float time);
            static void setUnixEpoch(std::int64_t);

        private:

            // Milliseconds
            static std::int64_t currUnixEpoch;
            static float elapsedTimeSeconds;

        
    };
}


#endif