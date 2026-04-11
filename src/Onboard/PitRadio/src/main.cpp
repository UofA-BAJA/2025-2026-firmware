// NEW PIT RADIO
// Based on RF95 example client from RadioHead

#include <SPI.h>
#include <RH_RF95.h>

const int RADIO_CS_PIN = 17;
const int RADIO_INT_PIN = 16;

// Singleton instance of the radio driver
RH_RF95 rf95(RADIO_CS_PIN, RADIO_INT_PIN);

void setTxPower(int8_t power, bool useRFO = false);

void setup()
{
    Serial.begin(115200);

    while (!Serial); // Wait for serial port to be available

    Serial.println("RF95 init in progress...");
    if (!rf95.init())
    {
        Serial.println("RF95 init failed. Try restarting power.");
        while (1)
            ;
    }

    rf95.setTxPower(23, false); // Sets maximum power for the LoRa module
    rf95.setModemConfig(RH_RF95::Bw500Cr45Sf128);

    // Defaults after init are 434.0MHz, 13dBm, Bw = 125 kHz, Cr = 4/5, Sf = 128chips/symbol, CRC on
    rf95.setFrequency(915.0);

    //Setup LoRa IDs, if we want to (probably should)
    // rf95.setThisAddress(1);
    // rf95.setHeaderTo(2);
    // rf95.setHeaderFrom(1);
    rf95.setHeaderFlags(0x00); // clear flags

    Serial.println("Init OK");
}

void loop()
{
    if (rf95.waitAvailableTimeout(2000))
    {
        // Should be a message for us now
        uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
        uint8_t len = sizeof(buf);

        
        if (rf95.recv(buf, &len))
        {
            int ind = 0;

            //Temporary parsing, eventually we're sending this raw via serial
            while(ind < len){

                //Make sure there's literally enough 
                if(len - ind < (2 + sizeof(float))){
                    break;
                }
            
                unsigned char id;
                memcpy(&id, buf + ind, 1);
                ind++;

                float timestamp;
                memcpy(&timestamp, buf+ind, sizeof(float));
                ind += sizeof(float);

                unsigned char dataLength;
                memcpy(&dataLength, buf+ind, 1);
                ind++;

                Serial.print("ID: ");
                Serial.println(id, HEX);
                Serial.print("Data Length: ");
                Serial.println(dataLength);
                Serial.print("Timestamp: ");
                Serial.println(timestamp);
                Serial.println("data 1 byte at a time below: ");


                for (int i = ind; i < ind + dataLength; i++)
                {
                    Serial.print(buf[i], HEX);
                    Serial.print(",");
                    
                }
                ind += dataLength;
                Serial.println();
            }
        }
        else
        {
            Serial.println("RX Failure");
        }
        Serial.print("Last SNR: ");
        Serial.println(rf95.lastSNR());
        Serial.println();
        Serial.println();
    }
    else
    {
        Serial.println("No Connection");
    }
    
}