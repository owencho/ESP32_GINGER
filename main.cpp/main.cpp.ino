//include file
#include "Arduino.h"
#include "LedStruct.h"
#include "Esp.h"
#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include <ArduinoJson.h>

const char* ssid = "VB77";
const char* password = "Bottas4LifeVB77";

LedStruct ledData[32];
WebServer server(80);

void generateOkReturn(void){
    DynamicJsonDocument doc(512);
    doc["status"] = "OK";
    
    String buf;
    serializeJson(doc, buf);
    server.send(201, F("application/json"), buf);
    Serial.print(F("\n done."));
}
void generateJsonError(DeserializationError error){
    // if the file didn't open, print an error:
    Serial.print(F("Error parsing JSON "));
    Serial.println(error.c_str());

    String msg = error.c_str();
    server.send(400, F("text/html"),
                "Error in parsin json body! " + msg);
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

void generateReturnError(char * status , char * message){
    DynamicJsonDocument doc(512);
    doc["status"] = status;
    doc["message"] = message;
    //send error message
    Serial.print(F("Stream..."));
    String buf;
    serializeJson(doc, buf);
    server.send(400, F("application/json"), buf);
    Serial.print(F("done."));
}
//setRoom
/*
void setLED() {
    String postBody = server.arg("plain");
    Serial.println(postBody);
    DynamicJsonDocument doc(512);
    DeserializationError error = deserializeJson(doc, postBody);
    if (error) {
      generateJsonError(error);
    } 
    else {
      JsonObject postObj = doc.as<JsonObject>();
      if (postObj.containsKey("address")) {
      
        Serial.println(F("done."));

        if(postObj.containsKey("red")){
          Serial.print(F("\n Red:"));
          Serial.printf(doc["red"]);
        }
        if(postObj.containsKey("green")){
          Serial.print(F("\n Green:"));
          Serial.printf(doc["green"]);
        }
        if(postObj.containsKey("blue")){
          Serial.print(F("\n Blue:"));
          Serial.printf(doc["blue"]);
        }
        /*
        // Here store data or doing operation
        if (!strcmp(doc["type"], "HIGH")) {
          digitalWrite(2, HIGH);
        }
        else if (!strcmp(doc["type"], "LOW")) {
          digitalWrite(2, LOW);
        }
        else {
          generateReturnError("KO","Error input");
          return;
        }
        
        //create return json data
        generateOkReturn();
      
      }
      else {
        generateReturnError("KO","Invalid input");
      }
    }
}

void setIntensity() {
    String postBody = server.arg("plain");
    Serial.println(postBody);
    DynamicJsonDocument doc(512);
    DeserializationError error = deserializeJson(doc, postBody);
    if (error) {
      generateJsonError(error);
    } 
    else {
      JsonObject postObj = doc.as<JsonObject>();
      if (postObj.containsKey("address")) {
       
        Serial.println(F("done."));
        
        if(postObj.containsKey("red")){
          Serial.print(F("\n Red:"));
          Serial.printf(doc["red"]);
        }
        if(postObj.containsKey("green")){
          Serial.print(F("\n Green:"));
          Serial.printf(doc["green"]);
        }
        if(postObj.containsKey("blue")){
          Serial.print(F("\n Blue:"));
          Serial.printf(doc["blue"]);
        }
        //create return json data
        generateOkReturn();
      
      }
      else {
        generateReturnError("KO","Invalid input");
      }
    }
}
*/
void setLedNode() {
    char intensity;
    String postBody = server.arg("plain");
    Serial.println(postBody);
    const size_t capacity = 4*JSON_OBJECT_SIZE(2) + JSON_OBJECT_SIZE(3) + 120;
    DynamicJsonDocument doc(capacity);
    DeserializationError error = deserializeJson(doc, postBody);
  
    if (error) {
      generateReturnMessage(400,"JSON malformed");
    } 
    
    else {
      JsonObject postObj = doc.as<JsonObject>();
      if (postObj.containsKey("address")) {
        if(postObj["address"] != 1){
          generateReturnMessage(404,"Address unavailable");
          return;
        }
        
        if(postObj.containsKey("led state")){
          if(postObj["led state"]["red"]){
            if(postObj["led state"]["red"]["power"]){
              if (!strcmp(postObj["led state"]["red"]["power"], "ON")) {
                digitalWrite(33, HIGH);
              }
              else if (!strcmp(postObj["led state"]["red"]["power"], "OFF")) {
                digitalWrite(33, LOW);
              }
            }
            if(postObj["led state"]["red"]["intensity"]){
            int redInt = postObj["led state"]["red"]["intensity"];
            Serial.print(F("\n red Intensity :"));
            Serial.print(redInt);
            }
          }
         
         if(postObj["led state"]["blue"]){
          if(postObj["led state"]["blue"]["power"]){
            if (!strcmp(postObj["led state"]["blue"]["power"], "ON")) {
              digitalWrite(2, HIGH);
            }
            else if (!strcmp(postObj["led state"]["blue"]["power"], "OFF")) {
              digitalWrite(2, LOW);
            }
          }
          if(postObj["led state"]["blue"]["intensity"]){
            int blueInt = postObj["led state"]["blue"]["intensity"];
            Serial.print(F("\n blue Intensity :"));
            Serial.print(blueInt);
          }
        }

        if(postObj["led state"]["green"]){
          if(postObj["led state"]["green"]["power"]){
            if (!strcmp(postObj["led state"]["green"]["power"], "ON")) {
              digitalWrite(32, HIGH);
            }
            else if (!strcmp(postObj["led state"]["green"]["power"], "OFF")) {
              digitalWrite(32, LOW);
            }
          }
          if(postObj["led state"]["green"]["intensity"]){
            int greenInt = postObj["led state"]["green"]["intensity"];
            Serial.print(F("\n green Intensity:"));
            Serial.print(greenInt);
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
//GET
/*
void getCurrent() {
    DynamicJsonDocument doc(512);
    String address;
    if (server.hasArg("address")) {
      address = server.arg("address");
      int addressValue=address.toInt();  
      if(ledData[addressValue].ledConnectionStatus){
          doc["address"] = address;
          doc["current"] = "1234";
          Serial.print(F("Stream..."));
          String buf;
          serializeJson(doc, buf);
          server.send(200, F("application/json"), buf);
          Serial.print(F("done."));
      }
      else{
          generateReturnError("KO","LED offline");
      }
    }
    else{
      generateReturnError("KO","Invalid args");
    }
}

void getVoltage() {
    DynamicJsonDocument doc(512);
    String address;
    if (server.hasArg("address")) {
      address = server.arg("address");
      int addressValue=address.toInt();  
      if(ledData[addressValue].ledConnectionStatus){
          doc["address"] = address;
          doc["voltage"] = "1234";
          Serial.print(F("Stream..."));
          String buf;
          serializeJson(doc, buf);
          server.send(200, F("application/json"), buf);
          Serial.print(F("done."));
      }
      else{
          generateReturnError("KO","LED offline");
      }
    }
    else{
      generateReturnError("KO","Invalid args");
    }
}

void getLEDStatus() {
    DynamicJsonDocument doc(512);
    String address;
    if (server.hasArg("address")) {
      address = server.arg("address");
      int addressValue=address.toInt();  
      if(ledData[addressValue].ledConnectionStatus){
          doc["name"] = ledData[addressValue].name;
          doc["address"] = address;
          doc["RED status"] = ledData[addressValue].r_status;
          doc["RED intensity"] = ledData[addressValue].r_intensity;
          doc["Green status"] = ledData[addressValue].g_status;
          doc["Green intensity"] = ledData[addressValue].g_intensity;
          doc["Blue status"] = ledData[addressValue].b_status;
          doc["Blue intensity"] = ledData[addressValue].b_intensity;
          Serial.print(F("Stream..."));
          String buf;
          serializeJson(doc, buf);
          server.send(200, F("application/json"), buf);
          Serial.print(F("done."));
      }
      else{
          generateReturnError("KO","LED offline");
      }
    }
    else{
      generateReturnError("KO","Invalid args");
    }
}
/*
void enableLED() {
    DynamicJsonDocument doc(512);
    String address;
    if (server.hasArg("address")) {
      address = server.arg("address");
      int addressValue=address.toInt();  
      if(addressValue >
      ledData[addressValue].ledConnectionStatus = ON;
    else{
      generateReturnError("KO","Invalid args");
    }
}
*/
//serve hello world
void getHelloWorld() {
    DynamicJsonDocument doc(512);
    doc["name"] = "Hello world";
  
    Serial.print(F("Stream..."));
    String buf;
    serializeJson(doc, buf);
    server.send(200, "application/json", buf);
    Serial.print(F("done."));
}

// Define routing
void restServerRouting() {
    server.on("/", HTTP_GET, []() {
      server.send(200, F("text/html"),
                  F("Welcome to the REST Web Server"));
    });
    //edit here to have the sub root folder
    //server.on(F("/v1/getCurrent"), HTTP_GET, getCurrent);
    //server.on(F("/v1/getVoltage"), HTTP_GET, getVoltage);
    //server.on(F("/v1/getLEDStatus"), HTTP_GET, getLEDStatus);

    //v1
    //server.on(F("/v1/setLED"), HTTP_POST, setLED);
    //server.on(F("/v1/setIntensity"), HTTP_POST, setIntensity);
    server.on(F("/v1/set_led_node"), HTTP_POST, setLedNode);
}

// Manage not found URL
void handleNotFound() {
    String message = "File Not Found\n\n";
    message += "URI: ";
    message += server.uri();
    message += "\nMethod: ";
    message += (server.method() == HTTP_GET) ? "GET" : "POST";
    message += "\nArguments: ";
    message += server.args();
    message += "\n";
    for (uint8_t i = 0; i < server.args(); i++) {
      message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
    }
    server.send(404, "text/plain", message);
}

void setup(void) {
    //init usart
    Serial.begin(115200);
    //configure wifi
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    Serial.println("");
  
    // Wait for connection
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }
    Serial.println("");
    Serial.print("Connected to ");
    Serial.println(ssid);
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
    
    //create root for json
    //set pin 2 LED as output
    pinMode(2, OUTPUT);
    pinMode(32, OUTPUT);
    pinMode(33, OUTPUT);
    // Set server routing
    restServerRouting();
    
    // Set not found response
    server.onNotFound(handleNotFound);
    // Start server
    
    server.begin();
    Serial.println("HTTP server started");
}

void loop(void) {
    server.handleClient();
}
