//include file
#include "Arduino.h"
#include "Esp.h"
#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include <ArduinoJson.h>

const char* ssid = "VB77";
const char* password = "Bottas4LifeVB77";
DynamicJsonDocument data1(512);
WebServer server(80);
//setRoom 
void setLED(){
    String postBody = server.arg("plain");
    Serial.println(postBody);
    DynamicJsonDocument doc(512);
    DeserializationError error = deserializeJson(doc, postBody);
    if (error) {
        // if the file didn't open, print an error:
        Serial.print(F("Error parsing JSON "));
        Serial.println(error.c_str());
        
        String msg = error.c_str();
        server.send(400, F("text/html"),
                "Error in parsin json body! " + msg);
 
    } else {
        JsonObject postObj = doc.as<JsonObject>();
        Serial.print(F("HTTP Method: "));
        Serial.println(server.method());
 
        if (server.method() == HTTP_POST) {
            if (postObj.containsKey("name") && postObj.containsKey("type")) {
 
                Serial.println(F("done."));
 
                // Here store data or doing operation
                if(!strcmp(doc["type"],"HIGH")){
                  digitalWrite(2,HIGH);
                }
                else if (!strcmp(doc["type"],"LOW")){
                  digitalWrite(2,LOW); 
                }
                else{
                  DynamicJsonDocument doc(512);
                  doc["status"] = "INVALID TYPE";
                  String buf;
                  serializeJson(doc, buf);
                  server.send(400, F("application/json"), buf);
                  Serial.print(F("\n done."));
                  return;
                }
                data1["type"] = doc["type"]; 
                
                Serial.print(F("\n Type:"));
                Serial.printf(doc["type"]);
            
                // Create the response
                // To get the status of the result you can get the http status so
                // this part can be unusefully
                DynamicJsonDocument doc(512);
                doc["status"] = "OK";
                String buf;
                serializeJson(doc, buf);
                server.send(201, F("application/json"), buf);
                Serial.print(F("\n done."));
 
            }
            else {
                DynamicJsonDocument doc(512);
                doc["status"] = "KO";
                doc["message"] = F("No data found, or incorrect!");
                //send error message
                Serial.print(F("Stream..."));
                String buf;
                serializeJson(doc, buf);
                server.send(400, F("application/json"), buf);
                Serial.print(F("done."));
            }
        }
    }
}
//getSetting
/*
void getLED(){
    StaticJsonDocument doc(512);
    Serial.print(F("Stream..."));
    String buf;
    serializeJson(doc, buf);
    server.send(200, F("application/json"), buf);
    Serial.print(F("done."));
}
*/
void getSettings(){
    DynamicJsonDocument doc(512);
    doc["ip"] = WiFi.localIP().toString();
    doc["gw"] = WiFi.gatewayIP().toString();
    doc["nm"] = WiFi.subnetMask().toString();
    if (server.arg("signalStrength")== "true"){
        doc["signalStrengh"] = WiFi.RSSI();
    }
    
    if (server.arg("freeHeap")== "true"){
        doc["freeHeap"] = ESP.getFreeHeap();
    }
    
    Serial.print(F("Stream..."));
    String buf;
    serializeJson(doc, buf);
    server.send(200, F("application/json"), buf);
    Serial.print(F("done."));
}
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
    server.on(F("/helloWorld"), HTTP_GET, getHelloWorld);
    server.on(F("/settings"), HTTP_GET, getSettings);
    //server.on(F("/getLED"), HTTP_GET, getLED);
    //POST
    server.on(F("/setLED"), HTTP_POST, setLED);
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
    
    // Activate mDNS this is used to be able to connect to the server
    // with local DNS hostmane esp32.local
    if (MDNS.begin("esp32")) {
      Serial.println("MDNS responder started");
    }
    //set pin 2 LED as output
    pinMode(2,OUTPUT);
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
