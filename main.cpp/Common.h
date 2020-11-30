#ifndef _Common_h
#define _Common_h
#include "Arduino.h"
#include <WebServer.h>
#include "LedStruct.h"
String getBooleanString(int value);
void generateReturnMessage(int statusCode , char * message);
#endif // Common_h