#include <Arduino.h>
#include "CANTProtocol.h"

typedef struct displacement{
  float left;
  float right;
} Displacement;
//5 is the CAN ID for one of the displacement sensors
//Change it to 6 for one of the sensors
CANTProtocol CAN(10, 2, 0x6);

void builder(unsigned char dataLength, byte* incomingData, unsigned long callbackID){
  Displacement x;
  x.left = analogRead(A2);
  x.right = analogRead(A3);
  CAN.sendRequestResponse(x, callbackID);
}


void setup() {
  Serial.begin(115200);
  CAN.registerRequest(0, builder);
  while(!CAN.begin()){
    delay(100);
    //CAN.end();
  }
  
}

void loop() {
  CAN.execute();
}

// put function definitions here:
