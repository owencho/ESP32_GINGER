#include "LedNodeProperties.h"
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

void getElectricalParameter() {
  DynamicJsonDocument doc(512);
  String address;
  if (server.hasArg("address")) {
    address = server.arg("address");
    int addressValue=address.toInt();  
    if(addressValue == 1){
      int voltageAdcValue = analogRead(34);
      int currentAdcValue = analogRead(35);
      double current = currentAdcValue * (5.0/4097.0);
      double voltage = voltageAdcValue * (80.0/4097.0);
      double power = current*voltage;
      doc["current"] = current;
      doc["voltage"] = voltage;
      doc["power"] = power;
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
