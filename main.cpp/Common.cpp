#include "Arduino.h"
#include "Common.h"
#include <ArduinoJson.h>
#include <WebServer.h>
extern WebServer server;

void generateReturnMessage(int statusCode , char * message){
    DynamicJsonDocument doc(JSON_OBJECT_SIZE(4));
    doc["status"] = statusCode;
    doc["message"] = message;
    //send error message
    Serial.print(F("\n Stream..."));
    String buf;
    serializeJson(doc, buf);
    server.send(statusCode, F("application/json"), buf);
    Serial.print(F("done. \n"));
}
