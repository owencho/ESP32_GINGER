//include file
#include "Arduino.h"
#include "LedStruct.h"
#include "Esp.h"
#include "SetLedNode.h"
#include "Common.h"
#include "LedNodeProperties.h"
#include <time.h>
#include <sys/time.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include <ArduinoJson.h>
using namespace std;
const char* ssid = "VB77";
const char* password = "Bottas4LifeVB77";
WebServer server(80);
void setTime() {
    int time[3];
    int previousLoc = 0;
    int currentLoc = 0;
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
      int valueSize;
      String strValue;
      if (postObj.containsKey("time")) {
        if(!postObj["time"].is<String>()){
          generateReturnMessage(400,"Json malformed");
        }
        String inputTime = postObj["time"];
        valueSize = inputTime.length();
        for(int i =0 ; i <3 ; i++){
          currentLoc = inputTime.indexOf(":",previousLoc+1);
          if(currentLoc == -1 && i != 2){
            generateReturnMessage(406,"Incorrect clock format");
            return;
          }
          if(i == 0){
            strValue = inputTime.substring(0,currentLoc);  
          }
          else if(i == 1){
            strValue = inputTime.substring(previousLoc+1,currentLoc);  
          }
          else{
            strValue = inputTime.substring(previousLoc+1,valueSize);
          }
          
          time[i] = strValue.toInt();
          previousLoc = currentLoc;
        }
        struct tm tm;
        tm.tm_hour = time[0];
        tm.tm_min = time[1];
        tm.tm_sec = time[2];
        time_t t = mktime(&tm);
        Serial.print(time[0]);
        Serial.print(time[1]);
        Serial.print(time[2]);
        struct timeval now = { .tv_sec = t };
        settimeofday(&now, NULL);
      }
   }
}

//GET

void getElectricalParameter() {
  DynamicJsonDocument doc(512);
  String address;
  if (server.hasArg("address")) {
    address = server.arg("address");
    int addressValue=address.toInt();  
    if(addressValue == 1){
      int voltageAdcValue = analogRead(34);
      int currentAdcValue = analogRead(35);
      Serial.print(voltageAdcValue);
      double current = currentAdcValue * (5.0/4097.0);
      double voltage = voltageAdcValue * (80.0/4097.0);
      double power = current*voltage;
      Serial.print(voltage);
      doc["current"] = current;
      doc["voltage"] = voltage; 
      doc["power"] = power;
      Serial.print(F("Stream..."));
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

// Define routing
void restServerRouting() {
    server.on("/", HTTP_GET, []() {
      server.send(200, F("text/html"),
                  F("Welcome to the REST Web Server"));
    });
    //edit here to have the sub root folder
    //GET
    server.on(F("/v1/get_electrical_parameter"), HTTP_GET, getElectricalParameter);
    server.on(F("/v1/get_properties"), HTTP_GET, getProperties);
    //POST
    server.on(F("/v1/set_led_node"), HTTP_POST, setLedNode);
    server.on(F("/v1/set_time"), HTTP_POST, setTime);
    server.on(F("/v1/set_properties"), HTTP_POST, setProperties);
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
