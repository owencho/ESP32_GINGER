#include "LedNodeProperties.h"
#include "Arduino.h"
#include "LedStruct.h"
#include "Esp.h"
#include <ArduinoJson.h>
#include <WebServer.h>
#include "Common.h"
extern WebServer server;
extern ControllerProperties ledControllerProperties;

void setProperties() {
  String postBody = server.arg("plain");
  Serial.println(postBody);
  const size_t capacity = 4*JSON_OBJECT_SIZE(2);
  DynamicJsonDocument doc(capacity);
  DeserializationError error = deserializeJson(doc, postBody);
  
  if (error) {
    generateReturnMessage(400,"JSON malformed");
  } 
  
  else {
    JsonObject postObj = doc.as<JsonObject>();
    if(postObj.containsKey("tag_with_time")){
        if(!postObj["tag_with_time"].is<boolean>()){
          generateReturnMessage(406,"Incorrect field type");
          return;
        }
        else{
          ledControllerProperties.tagWithTime = postObj["tag_with_time"];
        }
     }
     if(postObj.containsKey("overheating_recovery")){
        if(!postObj["overheating_recovery"].is<boolean>()){
          generateReturnMessage(406,"Incorrect field type");
          return;
        }
        else{
          ledControllerProperties.overHeatingRecovery = postObj["overheating_recovery"];
        }
     }
     generateReturnMessage(200,"OK");
  }
}

void getProperties() {
  DynamicJsonDocument doc(JSON_OBJECT_SIZE(4));
  doc["tag_with_time"] = ledControllerProperties.tagWithTime;
  doc["overheating_recovery"] = ledControllerProperties.overHeatingRecovery;
  String buf;
  serializeJson(doc, buf);
  server.send(200, F("application/json"), buf);
  Serial.print(F("done."));
}
