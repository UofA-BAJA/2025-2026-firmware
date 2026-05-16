// NEW PIT RADIO
// Based on RF95 example client from RadioHead

#include <SPI.h>
#include <RH_RF95.h>

const int RADIO_CS_PIN = 17; //17
const int RADIO_INT_PIN = 16;

const uint32_t USB_DELIMITER = 0xAAAAAAAA;

uint8_t outputBuf[RH_RF95_MAX_MESSAGE_LEN + 10];
uint16_t outputBufLength = 0;

uint8_t commandQueue[256];
uint8_t commandQueueLength = 0;
int newFrequency = -1;
uint8_t newDataTypeMask[32];

uint8_t radioOutputBuf[RH_RF95_MAX_MESSAGE_LEN];
uint16_t radioOuputBufLength = 0;

// Singleton instance of the radio driver
RH_RF95 rf95(RADIO_CS_PIN, RADIO_INT_PIN);

void setTxPower(int8_t power, bool useRFO = false);

void setup()
{
    Serial.begin(115200);

    while (!Serial); // Wait for serial port to be available
    pinMode(2, OUTPUT);
    // Serial.println("RF95 init in progress...");
    if (!rf95.init())
    {
        // Serial.println("RF95 init failed. Try restarting power.");
        while (1){
            digitalWrite(2, HIGH);
            delay(100);
            digitalWrite(2, LOW);
            delay(100);   
        }
    }

    rf95.setTxPower(23, false); // Sets maximum power for the LoRa module
    rf95.setModemConfig(RH_RF95::Bw500Cr45Sf128); //Sets maximum bandwidth
    rf95.setFrequency(915.0); //Sets 915 MHz so it's legal

    //Setup LoRa IDs, if we want to (probably should)
    // rf95.setThisAddress(1);
    // rf95.setHeaderTo(2);
    // rf95.setHeaderFrom(1);
    rf95.setHeaderFlags(0x00); // clear flags

}

//put these not on the stack
uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
uint8_t len = sizeof(buf);

uint8_t serialLastByte;
uint8_t intQueue[4];

bool flash = true;
bool stuffRemains = false;
bool acknowledged = false;
int sendIndex = 0;

void loop()
{
    if (rf95.waitAvailableTimeout(500))
    {
        digitalWrite(2, LOW);
        // Should be a message for us now
        len = sizeof(buf);
        if (rf95.recv(buf, &len))
        {
            int ind = 0;
                  
            uint8_t headerFlags = rf95.headerFlags();
            int16_t lastRSSI = rf95.lastRssi();

           

            //flags (1 byte) + rssi (2 bytes) + message (n bytes) + delimiters (4 bytes)
            outputBufLength = len + 7;

            memcpy(outputBuf, &USB_DELIMITER, 4);
            memcpy(outputBuf + 4, &headerFlags, 1);
            memcpy(outputBuf + 5, &lastRSSI, 2);
            memcpy(outputBuf + 7, buf, len);
            
            Serial.write(outputBuf, outputBufLength);
            
            // Serial.println(headerFlags, BIN);
            //Successful recieve, empty queue
            // Serial.println(headerFlags);
            if(headerFlags & (1 << 1) && !acknowledged){
                acknowledged = true;
                if(stuffRemains){
                    memcpy(commandQueue, commandQueue + sendIndex, commandQueueLength - sendIndex);
                    commandQueueLength = commandQueueLength - sendIndex;
                }else{
                    commandQueueLength = 0;                  
                }
            }
            if(headerFlags & 1){
                // Serial.println("trying to send");
                //Not everything was sent last time, try again
                if(!acknowledged){
                    // Serial.println("didn't empty buffer last time");
                    rf95.send(radioOutputBuf, radioOuputBufLength);
                }else{
                    // Serial.println("new stuff");
                    sendIndex = 0;
                    radioOuputBufLength = 0;
                    stuffRemains = false;
                    acknowledged = false;
                    for(; sendIndex < commandQueueLength; sendIndex++){
                        // Serial.println("actually adding commands");
                        if(commandQueue[sendIndex] == 0){
                            if(RH_RF95_MAX_MESSAGE_LEN - radioOuputBufLength > 4){
                                memcpy(radioOutputBuf + radioOuputBufLength, &newFrequency, 4);
                                radioOuputBufLength += 4;
                            }else{
                                stuffRemains = true;
                            }
                        }else if(commandQueue[sendIndex] == 1){
                            if(RH_RF95_MAX_MESSAGE_LEN - radioOuputBufLength > 32){
                                memcpy(radioOutputBuf + radioOuputBufLength, newDataTypeMask, 32);
                                radioOuputBufLength += 32;
                            }else{
                                stuffRemains = true;
                            }
                        }else if(radioOuputBufLength == RH_RF95_MAX_MESSAGE_LEN){
                            //shift everything down for the next cycle
                            stuffRemains = true;
                            break;
                        }else{
                            radioOutputBuf[radioOuputBufLength] = commandQueue[sendIndex];
                            radioOuputBufLength++;
                        }
                    }
                    
                    // int tempROBL = radioOuputBufLength;
                    rf95.send(radioOutputBuf, radioOuputBufLength);
                }
            }

            rf95.setHeaderFlags(0x0); //Clear header flags
        }
        else
        {
            // RX Failure (but not "nothing recieved")
        }
    }
    else
    {
        if(flash){
            digitalWrite(2, HIGH);
            flash = false;
        }else{
            digitalWrite(2, LOW);
            flash = true;
        }
        // Nothing Recieved
    }

    while(Serial.available() > 0){
        serialLastByte = (uint8_t)Serial.read();

        if(commandQueueLength < 256){
            //Change frequency
            if(serialLastByte == 0){
                int read = -1;
                for(int i = 0; i < 4; i++){
                    read = Serial.read();
                    if(read == -1) break; //if we read nothing, stop
                    intQueue[i] = (uint8_t)read;
                }
                memcpy(&newFrequency, intQueue, 4);
            }
            //Change Datatype mask
            else if(serialLastByte == 1){
                int read = -1;
                for(int i = 0; i < 32; i++){
                    read = Serial.read();
                    if(read == -1) break; //if we read nothing, stop
                    newDataTypeMask[i] = (uint8_t)read;
                }
            }

            //only add to queue at the end since we could be leaving malformed data otherwise
            commandQueue[commandQueueLength] = serialLastByte;
            commandQueueLength++;
        }
       

    }
    
}