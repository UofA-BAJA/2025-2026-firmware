#include <Arduino.h>
#include "CANTProtocol.h"

// 10 is D10, 2 is D2 
CANTProtocol CAN(10, 2, 0x7);


void builder(unsigned char dataLength, byte* incomingData, unsigned long callbackID) {
  float x = analogRead(A0);
  CAN.sendRequestResponse(x, callbackID);
}


void setup() {
  Serial.begin(115200);
  CAN.registerRequest(0, builder);
  while(!CAN.begin()) {
    delay(100);
  }

}

void loop() {
  CAN.execute();
}

// put function definitions here:
