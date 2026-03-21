/*
 * Here are the possible commands that will be sent from the pit computer
 * to the car computer.
*/

#ifndef COMMANDS_H
#define COMMANDS_H

enum Command{
    RADIO_FREQUENCY_CHANGE = 0,     // Reserved for the "frequency change" flag (radio command)
    RADIO_DATA_CHANGE = 1,          // Reserved for changing which data is being sent via radio

    DEFAULT_CAR_START = 2,          // Default command recieved when car starts (currently starts everything)
    START_LOG = 100,                // Starts recording data and realtime transmission
    END_LOG = 101,                  // Stops recording data and realtime transmission
    RETRIEVE = 102,                 // Sends the logs to the Pit Computer
    // We don't talk about 103
    EMERGENCY_STOP = 104,           // Not sure if we'll ever use this one
                                    // If we do, it would be a "crash the program" response 
    
    ENABLE_CHECK_ENGINE = 69,       // 24-25: enabled check engine light on dash
    DISABLE_CHECK_ENGINE = 70,      // 24-25: disabled check engine light on dash
    START_CAM = 105,                // testing?
    ACCELERATION = 106              // testing??

};


#endif