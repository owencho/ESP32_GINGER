extern "C"{
#include "UsartHardware.h"
#include "UsartDriver.h"
};
#include "ArduinoTXRX.h"
#include "Ticker.h"
#include "GetTemp.h"
#include "LedTime.h"
#include "Arduino.h"
#include "Common.h"
#include "LedStruct.h"
#include "Esp.h"
#include <ArduinoJson.h>
#include <WebServer.h>
#include "Common.h"
#include <time.h>
#include <sys/time.h>
#include <math.h>

extern WebServer server;
extern ControllerProperties ledControllerProperties;
FLOATUNION_t floatTemp;
int getTempPacket(int address,JsonObject& tempJsonObject){
    uint8_t * packet;
    uint8_t txPacket[3];
    double temperature;
    txPacket[0] = 3; //Command
    packet = transmitAndReceivePacket(2,&txPacket[0],address);
    
    if(packet){
      floatTemp.bytes[0] = packet[9];
      floatTemp.bytes[1] = packet[8];
      floatTemp.bytes[2] = packet[7];
      floatTemp.bytes[3] = packet[6];
      temperature = roundf(floatTemp.number * 100) / 100;
      Serial.print(F("temp value"));
      Serial.println(temperature);
      tempJsonObject["temp"] = temperature;
      if(packet[5]){
        tempJsonObject["overheating"] = true;
      }
      else{
        tempJsonObject["overheating"] = false;
      }
      resetUsartRxBuffer();
      return 1;
    }
    return 0;
    
}

void getLEDTemp() {
  DynamicJsonDocument doc(5*JSON_OBJECT_SIZE(2) + JSON_OBJECT_SIZE(4));
  String address;
  if (server.hasArg("address")) {
    address = server.arg("address");
    int addressValue=address.toInt();  
    if(addressValue == 1){
      JsonObject root = doc.to<JsonObject>();
      JsonObject red = root.createNestedObject("red");
      if(!getTempPacket(1,red)){
        generateReturnMessage(404,"Address unavailable");
        return;
      }
      doc["green"]["temp"]= 50;
      doc["green"]["overheating"]= false;
      doc["blue"]["temp"]= 20;
      doc["blue"]["overheating"]= false;
      if(ledControllerProperties.tagWithTime)  
        doc["clock"] =getTimeString();
        
      String buf;
      serializeJson(doc, buf);
      server.send(200, F("application/json"), buf);
      Serial.print(F("done."));
    }
    else{
      generateReturnMessage(404,"Address unavailable");
      return;
    }
  }
  else{
    generateReturnMessage(406,"No address given");
  }
}
