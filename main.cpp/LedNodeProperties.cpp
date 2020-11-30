#include "LedNodeProperties.h"
#include "Arduino.h"
#include "LedStruct.h"
#include "Esp.h"
#include <ArduinoJson.h>
#include <WebServer.h>
#include "Common.h"
WebServer server(80);
ControllerProperties ledControllerProperties;
void setProperties() {
    String postBody = server.arg("plain");
    Serial.println(postBody);
    const size_t capacity = JSON_OBJECT_SIZE(2);
    DynamicJsonDocument doc(capacity);
    DeserializationError error = deserializeJson(doc, postBody);
  
    if (error) {
      generateReturnMessage(400,"JSON malformed");
    } 
    
    else {
      JsonObject postObj = doc.as<JsonObject>();
      if(postObj.containsKey("tag_with_time")){
          if(!postObj["tag_with_time"].is<String>()){
            generateReturnMessage(406,"Incorrect field type");
            return;
          }
          else{
            if(postObj["tag_with_time"]== "true"){
              ledControllerProperties.tagWithTime = 1;
            }
            else if(postObj["tag_with_time"]== "false"){
              ledControllerProperties.tagWithTime = 0;
            }
            else{
              generateReturnMessage(406,"Incorrect field type");
              return;
            }
          }
       }
       if(postObj.containsKey("overheating_recovery")){
          if(!postObj["overheating_recovery"].is<String>()){
            generateReturnMessage(406,"Incorrect field type");
            return;
          }
          else{
            if(postObj["overheating_recovery"]== "true"){
              ledControllerProperties.overHeatingRecovery = 1;
            }
            else if(postObj["overheating_recovery"]== "false"){
              ledControllerProperties.overHeatingRecovery = 0;
            }
            else{
              generateReturnMessage(406,"Incorrect field type");
              return;
            }
          }
       }
       generateReturnMessage(200,"OK");
    }
}

void getProperties(){
  DynamicJsonDocument doc(512);
  doc["tag_with_time"] = getBooleanString(ledControllerProperties.tagWithTime);
  doc["overheating_recovery"] = getBooleanString(ledControllerProperties.overHeatingRecovery);
  String buf;
  serializeJson(doc, buf);
  server.send(200, F("application/json"), buf);
  Serial.print(F("done."));
}
