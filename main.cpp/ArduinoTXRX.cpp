extern "C" {
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

uint8_t * transmitAndReceivePacket(int size, uint8_t *data, int address) {
  uint8_t rxByte;
  int crc16;
  transmitPacket(size, data, address);
  timer1.attach_ms(2000, reTransmitPacket);

  while (!getPacketFromSlaves()) {
    if (Serial2.available() > 0 ) {
      rxByte = Serial2.read();
      /*
      Serial.println(F("received"));
      Serial.println(rxByte, HEX);
      */
      usartReceiveHardwareHandler(MAIN_CONTROLLER, rxByte);
    }
  
    if (retryCommand) {
      if (retryCounter < 2) {
        crc16 = getCRCValue();
        //Serial.println(F("crc"));
       // Serial.println(crc16);
        transmitPacket(size, data, address);
        Serial.println(F("resent packet "));
        retryCommand = 0;
        retryCounter++;
      }
      else {
        retryCommand = 0;
        retryCounter = 0;
        timer1.detach();
        setTxRS485();
        return NULL;
      }
    }
  }
  retryCommand = 0;
  retryCounter = 0;
  timer1.detach();
  setTxRS485();
  return getPacketFromSlaves();
}

void transmitPacket(int size, uint8_t *data, int address) {
  uint8_t txByte;
  setTxRS485();
  usartDriverTransmit(MAIN_CONTROLLER, address, size, data, NULL);
  while (!isTransmitLastByte()) {
    txByte = usartTransmitHardwareHandler(MAIN_CONTROLLER);
    //Serial.print(isTransmitLastByte());
    //Serial.println(txByte, HEX);
    Serial2.write(txByte);
  }
  Serial2.flush();
  setRxRS485();
  resetIsTransmitLastByte();
  Serial.println(F("packet Sent"));

}

void reTransmitPacket() {
  retryCommand = 1;
}

void setRxRS485() {
  digitalWrite(21, LOW);
}
void setTxRS485() {
  digitalWrite(21, HIGH);
}
