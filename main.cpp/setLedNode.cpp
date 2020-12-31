#include "SetLedNode.h"
#include "Arduino.h"
extern "C"{
#include "UsartHardware.h"
#include "UsartDriver.h"
};
#include "ArduinoTXRX.h"
#include "Ticker.h"
#include "LedStruct.h"
#include "Esp.h"
#include "Common.h"
#include <ArduinoJson.h>
#include <WebServer.h>
extern WebServer server;
extern ControllerProperties ledControllerProperties;

#define SET_INTENSITY_CMD 1
int status;
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
                status =setLedLightPower("red",postObj["led_state"]["red"]["power"],1);
                if(status == 0){
                    generateReturnMessage(404,"Address unavailable");
                    return;
                }
              }
            }

            if(postObj["led_state"]["red"]["intensity"]){
              if(!postObj["led_state"]["red"]["intensity"].is<int>()){
                generateReturnMessage(400,"Json malformed");
                return;
              }
              else{
                status =setLedLightIntensity("red",
                                           postObj["led_state"]["red"]["intensity"]
                                          ,1);
                if(!status){
                  generateReturnMessage(404,"Address unavailable");
                  return;
                }
              }
            }

            if(postObj["led_state"]["red"]["cut_off_temp"]){
              if(!postObj["led_state"]["red"]["cut_off_temp"].is<int>()){
                generateReturnMessage(400,"Json malformed");
                return;
              }
              else{
                status =setCutOffTemp("red",postObj["led_state"]["red"]["cut_off_temp"]
                                        ,1);
                if(!status){
                  generateReturnMessage(404,"Address unavailable");
                  return;
                }
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
                status =setLedLightPower("blue", postObj["led_state"]["blue"]["power"]
                ,2);
                if(status == 0){
                  generateReturnMessage(404,"Address unavailable");
                  return;
                }
              }
            }

            if(postObj["led_state"]["blue"]["intensity"]){
              if(!postObj["led_state"]["blue"]["intensity"].is<int>()){
                generateReturnMessage(400,"Json malformed");
                return;
              }
              else{
                int intensity = postObj["led_state"]["blue"]["intensity"];
                status =setLedLightIntensity("blue",
                                           intensity
                                          ,2);
                if(!status){
                  generateReturnMessage(404,"Address unavailable");
                  return;
                }
              }
            }

            if(postObj["led_state"]["blue"]["cut_off_temp"]){
              if(!postObj["led_state"]["blue"]["cut_off_temp"].is<int>()){
                generateReturnMessage(400,"Json malformed");
                return;
              }
              else{
                status =setCutOffTemp("blue",postObj["led_state"]["blue"]["cut_off_temp"]
                                        ,2);
                if(!status){
                  generateReturnMessage(404,"Address unavailable");
                  return;
                }
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
                status =setLedLightPower("green", postObj["led_state"]["green"]["power"]
                ,3);
                if(status == 0){
                  generateReturnMessage(404,"Address unavailable");
                  return;
                }
              }
            }

            if(postObj["led_state"]["green"]["intensity"]){
              if(!postObj["led_state"]["green"]["intensity"].is<int>()){
                generateReturnMessage(400,"Json malformed");
                return;
              }
              else{
                status =setLedLightIntensity("green",
                                           postObj["led_state"]["green"]["intensity"]
                                          ,3);
                if(!status){
                  generateReturnMessage(404,"Address unavailable");
                  return;
                }
              }
            }

            if(postObj["led_state"]["green"]["cut_off_temp"]){
              if(!postObj["led_state"]["green"]["cut_off_temp"].is<int>()){
                generateReturnMessage(400,"Json malformed");
                return;
              }
              else{
                status =setCutOffTemp("green",
                                      postObj["led_state"]["green"]["cut_off_temp"]
                                      ,3);
                if(!status){
                  generateReturnMessage(404,"Address unavailable");
                  return;
                }
              }
            }
          }
      }
       //create return json data
        generateReturnMessage(200,"OK");
    }
    else {
      generateReturnMessage(406,"No address given");
    }
  }
}

int setLedLightPower(String ledColor,String power,int address){
    uint8_t * packet;
    uint8_t txPacket[2];
    if (power == "ON")
      txPacket[1] = 1;
    else if (power == "OFF")
      txPacket[1] = 0;
    else
      return -1;
    
    txPacket[0] = 0; //Command
    packet = transmitAndReceivePacket(2,&txPacket[0],address);
    /*
    Serial.print(F("\n"));
    Serial.print(ledColor);
    Serial.print(F(" Power : "));
    Serial.print(power);
    */
    /*
    if(packet){
      Serial.println(packet[0]);
      Serial.println(packet[1]);
      Serial.println(packet[2]);
      Serial.println(packet[3]);
      Serial.println(packet[4]);
      Serial.println(packet[5]);
    }
    */
    if (packet){
      Serial.println(F("packet received"));
      resetUsartRxBuffer();
      return 1;
    }
    return 0;
}

int setCutOffTemp(String ledColor,int cutOffTemp,int address){
  uint8_t * packet;
  uint8_t txPacket[2];
  if (cutOffTemp < 110 && cutOffTemp > 0){
    txPacket[0] = 4; //Command
    txPacket[1] = cutOffTemp; 
    packet = transmitAndReceivePacket(2,&txPacket[0],address);
    /*
    Serial.print(F("\n"));
    Serial.print(ledColor);
    Serial.print(F(" cutOffTemp : "));
    Serial.print(cutOffTemp);
    */
    /*
    if(packet){
      Serial.println(packet[0]);
      Serial.println(packet[1]);
      Serial.println(packet[2]);
      Serial.println(packet[3]);
      Serial.println(packet[4]);
      Serial.println(packet[5]);
    }
    */
    if (packet){
      resetUsartRxBuffer();
      return 1;
    }
    return 0;
  }
  else
    return -1;
}

void setLedNodeName(String ledName,int address){
  Serial.print(F("\n"));
  Serial.print(ledName);
  Serial.print(F(" on Address "));
  Serial.print(address);
}

int setLedLightIntensity(String ledColor,int intensity,int address){
    uint8_t * packet;
    uint8_t ledIntensityTxPacket[3];
    if (intensity < 256 && intensity >= 0){
        ledIntensityTxPacket[0] = 1; //Command
        ledIntensityTxPacket[1] = (uint8_t)intensity;
        packet = transmitAndReceivePacket(2,&ledIntensityTxPacket[0],address);
        /*
        Serial.print(F("\n"));
        Serial.print(ledColor);
        Serial.println(F(" Intensity : "));
        Serial.print(intensity);
        */
        /*
        if(packet){
          Serial.println(packet[0]);
          Serial.println(packet[1]);
          Serial.println(packet[2]);
          Serial.println(packet[3]);
          Serial.println(packet[4]);
          Serial.println(packet[5]);
        }
        */
        if (packet){
          resetUsartRxBuffer();
          return 1;
        }
        return 0;
    }
    return 1;
}
