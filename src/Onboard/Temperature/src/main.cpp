#include <SPI.h>
#include <Wire.h>
#include <MLX90614.h>
#include <Arduino.h>
#include "CANTProtocol.h"

MLX90614 mlx = MLX90614(MLX90614_BROADCASTADDR);
CANTProtocol CAN(10, 2, 0x2);

void builder(unsigned char dataLength, byte* incomingData, unsigned long callbackID) {
  CAN.sendRequestResponse(mlx.readTemp(), callbackID);
}

void setup() {
  Wire.begin();
  Serial.begin(115200);
  CAN.registerRequest(0, builder);
  while(!mlx.begin()) {
    delay(100);
  }

  while (!CAN.begin()) {
    delay(100);
  }
}

void loop() {
  CAN.execute();
}

