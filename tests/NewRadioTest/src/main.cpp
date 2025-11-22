// rf95_client.pde
// -*- mode: C++ -*-
// Example sketch showing how to create a simple messageing client
// with the RH_RF95 class. RH_RF95 class does not provide for addressing or
// reliability, so you should only use RH_RF95 if you do not need the higher
// level messaging abilities.
// It is designed to work with the other example rf95_server
// Tested with Anarduino MiniWirelessLoRa, Rocket Scream Mini Ultra Pro with
// the RFM95W, Adafruit Feather M0 with RFM95

#include <SPI.h>
#include <RH_RF95.h>

const int RADIO_CS_PIN = 17;
const int RADIO_INT_PIN = 16;

// Singleton instance of the radio driver
RH_RF95 rf95(RADIO_CS_PIN, RADIO_INT_PIN);
//RH_RF95 rf95(5, 2); // Rocket Scream Mini Ultra Pro with the RFM95W
//RH_RF95 rf95(8, 3); // Adafruit Feather M0 with RFM95 

int mode = 0; //for testing: 0 = client, 1 = server


// Need this on Arduino Zero with SerialUSB port (eg RocketScream Mini Ultra Pro)
//#define Serial SerialUSB
void setTxPower(int8_t power, bool useRFO = false);

void setup() 
{
  // Rocket Scream Mini Ultra Pro with the RFM95W only:
  // Ensure serial flash is not interfering with radio communication on SPI bus
//  pinMode(4, OUTPUT);
//  digitalWrite(4, HIGH);
rf95.setTxPower(23, false); //Sets maximum power for the LoRa module
  Serial.begin(115200);


  while (!Serial) ; // Wait for serial port to be available
  if (!rf95.init()){
    Serial.println("init failed");
    while (1);
  }

  
  // Defaults after init are 434.0MHz, 13dBm, Bw = 125 kHz, Cr = 4/5, Sf = 128chips/symbol, CRC on
  rf95.setFrequency(915.0);
  
  // You can change the modulation parameters with eg
  // rf95.setModemConfig(RH_RF95::Bw500Cr45Sf128);
  
  // The default transmitter power is 13dBm, using PA_BOOST.
  // If you are using RFM95/96/97/98 modules which uses the PA_BOOST transmitter pin, then 
  // you can set transmitter powers from 2 to 20 dBm:
//  rf95.setTxPower(20, false);
  // If you are using Modtronix inAir4 or inAir9, or any other module which uses the
  // transmitter RFO pins and not the PA_BOOST pins
  // then you can configure the power transmitter power for 0 to 15 dBm and with useRFO true. 
  // Failure to do that will result in extremely low transmit powers.
//  rf95.setTxPower(14, true);
    Serial.println("***Which Radio is This?***\nEnter 0 for client, 1 for server.");
    while (!Serial.available());
    
    int input = Serial.parseInt();
    if(input == 0){
      mode = 0;
      Serial.println("Set to client");
      rf95.setThisAddress(1);
      rf95.setHeaderId(1); 
      rf95.setHeaderTo(2);
      rf95.setHeaderFrom(1);
      rf95.setHeaderFlags(0x00); //clear flags
      rf95.setHeaderFlags(0x01); //set flag to indicate client
    }else if(input == 1){
      mode = 1;
      rf95.setThisAddress(2);
      rf95.setHeaderId(2);
      rf95.setHeaderTo(1);
      rf95.setHeaderFrom(2);
      rf95.setHeaderFlags(0x00); //clear flags
      rf95.setHeaderFlags(0x02); //set flag to indicate server
      Serial.println("Set to server");
    }

}

void loop()
{
  if(mode == 0){
    //client
    Serial.println("Sending to rf95_server");
    // Send a message to rf95_server

    uint8_t data[] = "Hello World!";
    rf95.send(data, sizeof(data)); 

   
    
    // rf95.waitPacketSent();
    // // Now wait for a reply
    // uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
    // uint8_t len = sizeof(buf);

  //   if (rf95.waitAvailableTimeout(3000))
  //   { 
  //     // Should be a reply message for us now   
  //     if (rf95.recv(buf, &len))
  //   {
  //       Serial.print("got reply: ");
  //       Serial.println((char*)buf);
  // //      Serial.print("RSSI: ");
  // //      Serial.println(rf95.lastRssi(), DEC);    
  //     }
  //     else
  //     {
  //       Serial.println("recv failed");
  //     }
  //   }
  //   else
  //   {
  //     Serial.println("No reply, is rf95_server running?");
  //   }
    delay(50);








  }else if(mode == 1){
      if (rf95.available()) {
        // Should be a message for us now   
        uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
        uint8_t len = sizeof(buf);
        if (rf95.recv(buf, &len)){
          // digitalWrite(led, HIGH);
    //      RH_RF95::printBuffer("request: ", buf, len);
          Serial.print("got request: ");
          Serial.println((char*)buf);
          Serial.println(rf95.headerFrom());
          Serial.println(rf95.headerTo());
          Serial.println(rf95.headerId());
          Serial.println(rf95.headerFlags(), BIN);
        
    //      Serial.print("RSSI: ");
    //      Serial.println(rf95.lastRssi(), DEC);
        }
        else {
          Serial.println("recv failed");
        }
    }
  }
}