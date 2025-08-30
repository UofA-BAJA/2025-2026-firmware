#ifndef CANDEVICE_H
#define CANDEVICE_H

#include "CANDispatcher.h"
#include "Devices.h"
#include "Constants.h"


#include <functional>
#include <chrono>
#include <map>

namespace BajaWildcatRacing
{

    class CANDevice{

        private:
            CANDispatcher& m_canDispatcher;
            double minimumRepeatThreshold;
            byte deviceId;

            // Maps device + command bytes to the time when that command was sent
            std::unordered_map<uint16_t, std::chrono::steady_clock::time_point> activeCommandTimes;

            void populateValue(can_frame frame, void* destination);



        public:
            CANDevice(CANDispatcher& canDispatcher, byte deviceId) : m_canDispatcher(canDispatcher){
                this->minimumRepeatThreshold = 1/BASE_CAR_FREQUENCY * 1000;
                if((deviceId & ~(0b11111)) > 0){
                    std::cerr << "Error: deviceId " << std::bitset<8>(deviceId) << " is out of range (>5 bits)." << std::endl;
                    this->deviceId = 0;
                }else{
                    this->deviceId = deviceId;
                }
            }

        protected:
            void sendCanRequest(byte dataType, std::vector<byte> data, void* receivedData, int recievedDataLength);
            void sendCanRequest(byte dataType, void* receivedData, int receivedDataLength);
            void sendLossyCanCommand(byte dataType, std::vector<byte> data);
            void sendLossyCanCommand(byte dataType);
            void sendLosslessCanCommand(byte dataType, std::vector<byte> data);
            void sendLosslessCanCommand(byte dataType);
            

    };
    
}



#endif