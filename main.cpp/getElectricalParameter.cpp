#include "getElectricalParameter.h"
#include "LedTime.h"
#include "Arduino.h"
#include "LedStruct.h"
#include "Esp.h"
#include <ArduinoJson.h>
#include <WebServer.h>
#include "Common.h"
#include <time.h>
#include <sys/time.h>
#include <iterator>
#include <set>
using namespace std;
extern WebServer server;
extern ControllerProperties ledControllerProperties;
extern set<int> ledSet;

void getElectricalParameter() {
  DynamicJsonDocument doc(JSON_OBJECT_SIZE(5));
  String address;
  if (server.hasArg("address")) {
    address = server.arg("address");
    int addressValue=address.toInt();  
    if(addressValue == 1){
      double current = getCurrent(addressValue);
      double voltage = getVoltage(addressValue);
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
double getCurrent(int address){
  int currentAdcValue = analogRead(35);
  return currentAdcValue * (5.0/4097.0);
}

double getVoltage(int address){
  int voltageAdcValue = analogRead(34);
  return voltageAdcValue * (80.0/4097.0);
}
