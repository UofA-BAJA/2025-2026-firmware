// rf95_client.pde
// -*- mode: C++ -*-
// Example sketch showing how to create a simple messageing client
// with the RH_RF95 class. RH_RF95 class does not provide for addressing or
// reliability, so you should only use RH_RF95 if you do not need the higher
// level messaging abilities.
// It is designed to work with the other example rf95_server
// Tested with Anarduino MiniWirelessLoRa, Rocket Scream Mini Ultra Pro with
// the RFM95W, Adafruit Feather M0 with RFM95

// #include <Arduino.h>


#include <RH_RF95.h>



const int RADIO_CS_PIN = 11;
const int RADIO_INT_PIN = 21;

RH_RF95 rf95(RADIO_CS_PIN, RADIO_INT_PIN);


int mode = 0; //for testing: 0 = client, 1 = server

int main(){
  std::cout << "Piduino init in progress..." << std::endl;
  /*
    Below code is modified from Piduino.h
    We don't want the piduino setup() and loop() stuff, but we do want the arduino-style I/O setup
  */
  
  // ::HardwareSerial::setupAvailablePorts();

  /*
    End code copied from piduino
  */

  std::cout << "Radio init in progress..." << std::endl;

  if (!rf95.init()){
    std::cout << "Radio init failed :(" << std::endl;
    exit(EXIT_FAILURE);
  }

  rf95.setFrequency(915.0);
  rf95.setTxPower(23, false); //Sets maximum power for the LoRa module

  rf95.setThisAddress(1);
  rf95.setHeaderId(1); 
  rf95.setHeaderTo(2);
  rf95.setHeaderFrom(1);
  rf95.setHeaderFlags(0x00); //clear flags
  rf95.setHeaderFlags(0x01);
  
  std::cout << "Initialization complete" << std::endl;
  for(;;) {
    std::cout << "Sending to rf95 server" << std::endl;
    // Send a message to rf95_server

    uint8_t data[] = "Hello World!";
    if(!rf95.send(data, sizeof(data))){
      std::cout << "failed to send" << std::endl;
    }
    delay(50);
  }  
}

