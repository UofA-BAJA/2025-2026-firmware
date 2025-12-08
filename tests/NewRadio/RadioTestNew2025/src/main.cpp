#include <Arduino.h>
#include <SPI.h>
#include <RH_RF95.h>

// Define the frequency for your RFM95W module (e.g., 915.0MHz for US, 868.0MHz for EU)
#define RF95_FREQ 915.0

// Create an instance of the RH_RF95 driver
RH_RF95 rf95;

void setup() {
  Serial.begin(9600);
  while (!Serial); // Wait for serial port to be available

  Serial.println("Feather LoRa TX Test!");

  // Initialize the RFM95 module
  if (!rf95.init()) {
    Serial.println("LoRa radio init failed");
    while (1);
  }
  Serial.println("LoRa radio init OK!");

  // Set the frequency
  if (!rf95.setFrequency(RF95_FREQ)) {
    Serial.println("setFrequency failed");
    while (1);
  }
  Serial.print("Set Freq to: ");
  Serial.println(RF95_FREQ);

  // Set the transmit power (5-23dBm for PA_BOOST)
  rf95.setTxPower(23, false);
}

int16_t packetnum = 0; // packet counter

void loop() {
  delay(1000); // Wait 1 second between transmits

  Serial.println("Transmitting...");

  char radiopacket[20] = "Hello World # ";
  itoa(packetnum++, radiopacket + 13, 10); // Convert packetnum to string and append
  Serial.print("Sending ");
  Serial.println(radiopacket);

  rf95.send((uint8_t *)radiopacket, 20); // Send the packet
  rf95.waitPacketSent(); // Wait for the packet to be sent

  Serial.println("Packet sent!");
}