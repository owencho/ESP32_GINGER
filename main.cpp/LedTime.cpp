#include "LedTime.h"
#include "Arduino.h"
#include "LedStruct.h"
#include "Esp.h"
#include <ArduinoJson.h>
#include <WebServer.h>
#include "Common.h"
#include <time.h>
#include <sys/time.h>
extern WebServer server;
extern ControllerProperties ledControllerProperties;

void setTime() {
  int time[3];
  int previousLoc = 0;
  int currentLoc = 0;
  String postBody = server.arg("plain");
  Serial.println(postBody);
  const size_t capacity = 2*JSON_OBJECT_SIZE(1) + 20;
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
      int t = time[2]+(time[1]+time[0]*60)*60;
      struct timeval now = { .tv_sec = t };
      settimeofday(&now, NULL);
      generateReturnMessage(200,"OK");
    }
  }
}

String getTimeString(){
  struct tm timeDetails;
  time_t now;
  time(&now);
  localtime_r(&now, &timeDetails);
  String timeOutput = String(timeDetails.tm_hour) + ':'+String(timeDetails.tm_min)+':'+String(timeDetails.tm_sec);
  return timeOutput;
}
