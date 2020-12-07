#include "SetLedNode.h"
#include "Arduino.h"
#include "LedStruct.h"
#include "Esp.h"
#include "Common.h"
#include <ArduinoJson.h>
#include <WebServer.h>
extern WebServer server;
extern ControllerProperties ledControllerProperties;
void setLedNode() {
    char intensity;
    String postBody = server.arg("plain");
    Serial.println(postBody);
    const size_t capacity = 5*JSON_OBJECT_SIZE(3) + 90;
    DynamicJsonDocument doc(capacity);
    DeserializationError error = deserializeJson(doc, postBody);
  
    if (error) {
      generateReturnMessage(400,"JSON malformed");
    } 
    
    else {
      JsonObject postObj = doc.as<JsonObject>();
      if (postObj.containsKey("address")) {
        if(!postObj["address"].is<int>()){
          generateReturnMessage(400,"Json malformed");
          return;
        }
        else if(postObj["address"] != 1){
          generateReturnMessage(404,"Address unavailable");
          return;
        }
        
        if(postObj.containsKey("name")){
          if(!postObj["name"].is<String>()){
            generateReturnMessage(400,"Json malformed");
            return;
          }
          else
            setLedNodeName(postObj["name"],postObj["address"]);
        }
        
        if(postObj.containsKey("led_state")){
          if(postObj["led_state"]["red"]){
            
            if(postObj["led_state"]["red"]["power"]){
              if(!postObj["led_state"]["red"]["power"].is<String>()){
                generateReturnMessage(400,"Json malformed");
                return;
              }  
              else{
                setLedLightPower(postObj["led_state"]["red"]["power"],33);
              }
            }
            
            if(postObj["led_state"]["red"]["intensity"]){
              if(!postObj["led_state"]["red"]["intensity"].is<int>()){
                generateReturnMessage(400,"Json malformed");
                return;
              }
              else{
                setLedLightIntensity("red",postObj["led_state"]["red"]["intensity"]
                                      ,postObj["address"]);
              }
            }
            
            if(postObj["led_state"]["red"]["cut_off_temp"]){
              if(!postObj["led_state"]["red"]["cut_off_temp"].is<int>()){
                generateReturnMessage(400,"Json malformed");
                return;
              }
              else{
                setCutOffTemp("red",postObj["led_state"]["red"]["cut_off_temp"]
                                      ,postObj["address"]);
              }
            }
          }
          
          if(postObj["led_state"]["blue"]){
            
            if(postObj["led_state"]["blue"]["power"]){
              if(!postObj["led_state"]["blue"]["power"].is<String>()){
                generateReturnMessage(400,"Json malformed");
                return;
              }  
              else{
                setLedLightPower(postObj["led_state"]["blue"]["power"],2);
              }
            }
            
            if(postObj["led_state"]["blue"]["intensity"]){
              if(!postObj["led_state"]["blue"]["intensity"].is<int>()){
                generateReturnMessage(400,"Json malformed");
                return;
              }
              else{
                setLedLightIntensity("blue",postObj["led_state"]["blue"]["intensity"]
                                      ,postObj["address"]);
              }
            }
            
            if(postObj["led_state"]["blue"]["cut_off_temp"]){
              if(!postObj["led_state"]["blue"]["cut_off_temp"].is<int>()){
                generateReturnMessage(400,"Json malformed");
                return;
              }
              else{
                setCutOffTemp("blue",postObj["led_state"]["blue"]["cut_off_temp"]
                                      ,postObj["address"]);
              }
            }
          }
          
          if(postObj["led_state"]["green"]){
            if(postObj["led_state"]["green"]["power"]){
              if(!postObj["led_state"]["green"]["power"].is<String>()){
                generateReturnMessage(400,"Json malformed");
                return;
              }  
              else{
                setLedLightPower(postObj["led_state"]["green"]["power"],32);
              }
            }
            
            if(postObj["led_state"]["green"]["intensity"]){
              if(!postObj["led_state"]["green"]["intensity"].is<int>()){
                generateReturnMessage(400,"Json malformed");
                return;
              }
              else{
                setLedLightIntensity("green",postObj["led_state"]["green"]["intensity"]
                                      ,postObj["address"]);
              }
            }
            
            if(postObj["led_state"]["green"]["cut_off_temp"]){
              if(!postObj["led_state"]["green"]["cut_off_temp"].is<int>()){
                generateReturnMessage(400,"Json malformed");
                return;
              }
              else{
                setCutOffTemp("green",postObj["led_state"]["green"]["cut_off_temp"]
                              ,postObj["address"]);
              }
            }
          }
      }
       //create return json data
        //delay(3000);
        generateReturnMessage(200,"OK");
    }
    else {
      generateReturnMessage(406,"No address given");
    }
  }
}
void setLedLightPower(String power,int address){
  if (power == "ON")
    digitalWrite(address, HIGH);
  else if (power == "OFF")
    digitalWrite(address, LOW);
  Serial.print(power);
}

void setCutOffTemp(String ledColor,int cutOffTemp,int address){
  if (cutOffTemp < 110 && cutOffTemp > 0){
    Serial.print(F("\n"));
    Serial.print(ledColor);
    Serial.print(F(" cut off temperature : "));
    Serial.print(cutOffTemp);
    Serial.print(F(" On address "));
    Serial.print(address); 
  }
}

void setLedNodeName(String ledName,int address){
  Serial.print(F("\n"));
  Serial.print(ledName);
  Serial.print(F(" on Address "));
  Serial.print(address);
}

void setLedLightIntensity(String ledColor,int intensity,int address){
  if (intensity < 256 && intensity >= 0){
    Serial.print(F("\n"));
    Serial.print(ledColor);
    Serial.print(F(" Intensity : "));
    Serial.print(intensity);
    Serial.print(F(" On address "));
    Serial.print(address);
  }
}
