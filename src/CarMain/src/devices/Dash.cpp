#include "Dash.h"


namespace BajaWildcatRacing
{

    Dash::Dash(CANDispatcher& canDispatcher, byte deviceId) : CANDevice(canDispatcher, deviceId){
        
    }

    // Sends latest RPM information to the Dash
    void Dash::sendRPM(float rpm){
        std::vector<byte> data(sizeof(float), 0);

        memcpy(data.data(), &rpm, sizeof(float));

        sendLossyCanCommand(0x00, data);

    }

    // Sends latest speed information to the Dash
    void Dash::sendSpeed(float speed){
        std::vector<byte> data(sizeof(float), 0);

        memcpy(data.data(), &speed, sizeof(float));

        sendLossyCanCommand(0x01, data);
        
    }

    // Sends latest CVT Temp information to the Dash
    void Dash::sendCVTTemp(float cvtTemp){
        std::vector<byte> data(sizeof(float), 0);

        memcpy(data.data(), &cvtTemp, sizeof(float));
        
        sendLossyCanCommand(0x02, data);
        
    }

    // Sends latest Time Elapsed information to the Dash
    void Dash::sendTimeSeconds(float seconds){
        std::vector<byte> data(sizeof(float), 0);
        
        memcpy(data.data(), &seconds, sizeof(float));
        
        sendLossyCanCommand(0x03, data);

    }

    //Sends latest distance infromation to the dash
    void Dash::sendDistance(float distance){
        std::vector<byte> data(sizeof(float), 0);
        
        memcpy(data.data(), &distance, sizeof(float));
        
        sendLossyCanCommand(0x04, data);
    }
}
