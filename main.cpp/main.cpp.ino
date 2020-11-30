//include file
#include "Arduino.h"
#include "LedStruct.h"
#include "Esp.h"
#include "SetLedNode.h"
#include "Common.h"
#include "LedNodeProperties.h"
#include "LedTime.h"
#include "getElectricalParameter.h"
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
ControllerProperties ledControllerProperties;
LedStruct

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
