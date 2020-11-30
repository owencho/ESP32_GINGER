#include "Arduino.h"
#include "Common.h"
#include <ArduinoJson.h>
extern WebServer server;
String getBooleanString(int value){
  if(value){
    return "true";
  }
  else{
    return "false";
  }
}

void generateReturnMessage(int statusCode , char * message){
    DynamicJsonDocument doc(512);
    doc["status"] = statusCode;
    doc["message"] = message;
    //send error message
    Serial.print(F("\n Stream..."));
    String buf;
    serializeJson(doc, buf);
    server.send(statusCode, F("application/json"), buf);
    Serial.print(F("done. \n"));
}
