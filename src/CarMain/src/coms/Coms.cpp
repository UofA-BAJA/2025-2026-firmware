/*

comments
comments
comments


*/

#define RADIO_CS_PIN 11
#define RADIO_INT_PIN 21

#include "Coms.h"

namespace BajaWildcatRacing {

    Coms::Coms(ProcedureScheduler& procedureScheduler)
    :rf95(RADIO_CS_PIN, RADIO_INT_PIN)
    ,procedureScheduler(procedureScheduler) {
        radioThread = std::thread(&Coms::executeRadio, this);
    }


    void Coms::execute(float timestamp){

    }

    void Coms::end(){

    }

    void Coms::executeRadio(){
        //init radio 
        if (!rf95.init()){
            std::cout << "RF95 is not responding!!!" << std::endl;
            //TODO: log error/panic
            running = false;
            return;
        }

        rf95.setFrequency(915.0);
        rf95.setTxPower(23, false); //Sets maximum power for the LoRa module

        //TODO: define what the addresses we are using are
        rf95.setThisAddress(1);
        rf95.setHeaderFrom(1);
        rf95.setHeaderTo(2);
    

        int waitTimems = (1.0 / RADIO_CLOCK_FREQUENCY) * 1000;
        while(running){
            if(currentPitCommandState == PitCommandState::IDLE){
                idle();
            }
            else if(currentPitCommandState == PitCommandState::LIVE_DATA_TRANSMIT){
                transmitLiveData();
            }
            else if(currentPitCommandState == PitCommandState::WAIT_COMMAND_RECIEVE){
                recieveCommand();
            }

            
            std::this_thread::sleep_for(std::chrono::milliseconds(waitTimems));
        }
        //run correct function depending on state
        //if live_data_transmit, send special packet and switch to recievecommand once every second 
    }

    void Coms::transmitLiveData(){
        //take stuff from queue and transmit as fast as possible
    }

    void Coms::recieveCommand(){
        //check for RX on radio with short timeout
        //do procedurescheduler command / radio command (or nothing)
        //switch back to LIVE_DATA_TRANSMIT (or IDLE if command is to go to IDLE)
    }

    void Coms::idle(){
        //listen for commands
        //this is different to setting running = false 
    }

    void Coms::sendData(DataTypes dataType, float data){
        //Get current timestamp
        //Save data into a data queue
        //Maybe a way to throttle the live data update?
        //In the future, maybe have a state as to what datatypes we want and only enqueue those?
    }
}