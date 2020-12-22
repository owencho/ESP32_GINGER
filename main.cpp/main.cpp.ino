//include file
extern "C"{
#include "Event.h"
#include "CommEventQueue.h"
#include "StateMachine.h"
#include "UsartHardware.h"
#include "UsartDriver.h"
#include "EventQueue.h"
};

#include "driver/uart.h"
#include "Arduino.h"
#include "ArduinoTXRX.h"
#include "GetTemp.h"
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
#include <iterator>
#include <set>
#include "Ticker.h"
using namespace std;
const char* ssid = "VB77";
const char* password = "Bottas4LifeVB77";
Ticker timer1;
set<int> ledSet;
WebServer server(80);
ControllerProperties ledControllerProperties;
/*
void getLedNode(){
    DynamicJsonDocument doc(512);
    doc["status"][0] = 1;
    doc["status"][1] = 2;
    //send error message
    Serial.print(F("\n Stream..."));
    String buf;
    serializeJson(doc, buf);
    server.send(200, F("application/json"), buf);
    Serial.print(F("done. \n"));
}
*/
// Define routing
void restServerRouting() {
  server.on("/", HTTP_GET, []() {
  server.send(200, F("text/html"),
              F("Welcome to the REST Web Server"));
  });
  //edit here to have the sub root folder
  //GET
  server.on(F("/v1/get_electrical_parameter"), HTTP_GET, getElectricalParameter);
  server.on(F("/v1/get_temperature"), HTTP_GET, getLEDTemp);
  server.on(F("/v1/get_properties"), HTTP_GET, getProperties);
  //server.on(F("/v1/get_led_node"), HTTP_GET, getLedNode);
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
/*
QueueHandle_t uart_queue;
void configureUART2(){
    const int uart_num = UART2;
    uart_config_t uart_config = {
    .baud_rate = 9600,
    .data_bits = UART_DATA_8_BITS,
    .parity = UART_PARITY_DISABLE,
    .stop_bits = UART_STOP_BITS_1,
    .flow_ctrl = UART_HW_FLOWCTRL_CTS_RTS,
    .rx_flow_ctrl_thresh = 122,
    };
    // Configure UART parameters
    ESP_ERROR_CHECK(uart_param_config(uart_num, &uart_config));
    //config pin
    ESP_ERROR_CHECK(uart_set_pin(UART_NUM_2, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE, 18, 19));
    //driver installation
    // Setup UART buffered IO with event queue
    const int uart_buffer_size = (1024 * 2);
    // Install UART driver using an event queue here
    ESP_ERROR_CHECK(uart_driver_install(UART2, uart_buffer_size,uart_buffer_size, 10, &uart_queue, 0));
}
*/
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
  Serial.println("Connected to WiFi");
  //Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  if(!MDNS.begin("esp32")) {
       Serial.println("Error starting mDNS");
       return;
  }
  //configureUART2();
  Serial2.begin(9600, SERIAL_8N1, 16, 17);
  pinMode (17 , INPUT_PULLUP);
  //create root for json
  //pin for rs485
  pinMode(32, OUTPUT);
  pinMode(21, OUTPUT);
  //set tx mode
  setTxRS485();
  // Set server routing
  restServerRouting();

  // Set not found response
  server.onNotFound(handleNotFound);
  // Start server
  server.begin();
  Serial.println("HTTP server started");
  usartInit();
}
int rxByte;
void loop(void) {
  /*
      if (Serial2.available() > 0 ) {
      rxByte = Serial2.read();
      Serial.println(F("received"));
      Serial.println(rxByte, HEX);
    }
    */
  //Serial2.write(170);
  server.handleClient();
  
}
