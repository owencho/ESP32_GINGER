extern "C"{
#include "Event.h"
#include "CommEventQueue.h"
#include "StateMachine.h"
#include "UsartHardware.h"
#include "UsartDriver.h"
#include "EventQueue.h"
};
#include "ArduinoTXRX.h"
#include "Arduino.h"
#include "Esp.h"
#include "Common.h"

#include <time.h>
#include <sys/time.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include <ArduinoJson.h>
#include <iterator>
#include <set>
#include "Ticker.h"

extern Ticker timer1;
uint8_t retryCounter;
uint8_t retryCommand;

uint8_t * transmitAndReceivePacket(int size,uint8_t *data,int address){
    uint8_t rxByte;
    transmitPacket(size,data,address);
    timer1.attach_ms(200, reTransmitPacket);
    setRxRS485();
    while(!getPacketFromSlaves()){
        rxByte = Serial2.read();
        if(rxByte != -1){
          usartReceiveHardwareHandler(MAIN_CONTROLLER,rxByte);
        }
        if(retryCommand){
            if(retryCounter < 2){
                transmitPacket(size,data,address);
                setRxRS485();
                Serial.print(F("resent"));
                retryCommand =0;
                retryCounter++;
            }
            else{
                retryCommand =0;
                retryCounter = 0;
                timer1.detach();
                return NULL;
            }
        }
    }
    retryCommand = 0;
    retryCounter = 0;
    timer1.detach();
    return getPacketFromSlaves();
}

void transmitPacket(int size,uint8_t *data,int address){
    uint8_t txByte;
    usartDriverTransmit(MAIN_CONTROLLER,address,size,data,NULL);
    while(!isTransmitLastByte()){
        digitalWrite(21, HIGH);
        digitalWrite(19,LOW);
        txByte = usartTransmitHardwareHandler(MAIN_CONTROLLER);
        Serial.print(isTransmitLastByte());
        Serial2.write(txByte);
    }
    resetIsTransmitLastByte();
    Serial.print(F("successfully sent"));
}

void reTransmitPacket(){
    retryCommand = 1;
}

void setRxRS485(){
    digitalWrite(21, LOW);
    digitalWrite(19,HIGH);
}
void setTxRS485(){
    digitalWrite(21, HIGH);
    digitalWrite(19,LOW);
}
