extern "C"{
#include "UsartHardware.h"
#include "UsartDriver.h"
};
#include "ArduinoTXRX.h"
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
#include <math.h>

using namespace std;
extern WebServer server;
extern ControllerProperties ledControllerProperties;
extern set<int> ledSet;

FLOATUNION_t floatValue;
int getElectricParamPacket(int address,JsonObject& tempJsonObject){
    uint8_t * packet;
    uint8_t txPacket[3];
    double current;
    double voltage;
    txPacket[0] = 2; //Command
    packet = transmitAndReceivePacket(2,&txPacket[0],address);
   
    if(packet){
      floatValue.bytes[0] = packet[8];
      floatValue.bytes[1] = packet[7];
      floatValue.bytes[2] = packet[6];
      floatValue.bytes[3] = packet[5];
      voltage = roundf(floatValue.number * 100) / 100;
      Serial.print(F("voltage value"));
      Serial.println(voltage);
      floatValue.bytes[0] = packet[12];
      floatValue.bytes[1] = packet[11];
      floatValue.bytes[2] = packet[10];
      floatValue.bytes[3] = packet[9];
      current = roundf(floatValue.number * 100) / 100;
      Serial.print(F("current value"));
      Serial.println(current);
      tempJsonObject["current"] = current;
      tempJsonObject["voltage"] = voltage;
      tempJsonObject["power"] = current*voltage;
      resetUsartRxBuffer();
      return 1;
    }
    return 0;
}

void getElectricalParameter() {
  DynamicJsonDocument doc(5*JSON_OBJECT_SIZE(2) + JSON_OBJECT_SIZE(4));
  String address;
  if (server.hasArg("address")) {
    address = server.arg("address");
    int addressValue=address.toInt();  
    if(addressValue == 1){
      JsonObject root = doc.to<JsonObject>();
      JsonObject red = root.createNestedObject("red");
      if(!getElectricParamPacket(1,red)){
        generateReturnMessage(404,"Address unavailable");
        return;
      }
      doc["green"]["current"]= 50;
      doc["green"]["voltage"]= 1;
      doc["green"]["power"]= 50;
      doc["blue"]["current"]= 50;
      doc["blue"]["voltage"]= 1;
      doc["blue"]["power"]= 50;

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
