#include "GetTemp.h"
#include "LedTime.h"
#include "Arduino.h"
#include "LedStruct.h"
#include "Esp.h"
#include <ArduinoJson.h>
#include <WebServer.h>
#include "Common.h"
#include <time.h>
#include <sys/time.h>

extern WebServer server;
extern ControllerProperties ledControllerProperties;

DynamicJsonDocument getTempValue(int address){
  DynamicJsonDocument doc(5*JSON_OBJECT_SIZE(2) + JSON_OBJECT_SIZE(4));
  doc["red"]["temp"]= 101;
  doc["red"]["overheating"]= true;
  doc["green"]["temp"]= 50;
  doc["green"]["overheating"]= false;
  doc["blue"]["temp"]= 20;
  doc["blue"]["overheating"]= false;
  return doc;
}
void getLEDTemp() {
  DynamicJsonDocument doc(1);
  String address;
  if (server.hasArg("address")) {
    address = server.arg("address");
    int addressValue=address.toInt();  
    if(addressValue == 1){
      doc = getTempValue(addressValue);
      if(ledControllerProperties.tagWithTime)  
        doc["clock"] =getTimeString();
      String buf;
      serializeJson(doc, buf);
      server.send(200, F("application/json"), buf);
      Serial.print(F("done."));
    }
    else{
      generateReturnMessage(404,"Address unavailable");
    }
  }
  else{
    generateReturnMessage(406,"No address given");
  }
}
