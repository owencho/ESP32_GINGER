#ifndef _SetLedNode_h
#define _SetLedNode_h
#include "Arduino.h"
void setLedNode();
void setLedLightPower(String power,int address);
void setLedLightIntensity(String ledColor,int intensity,int address);
void setCutOffTemp(String ledColor,int cutOffTemp,int address);
void setLedNodeName(String ledName,int address);
#endif // SetLedNode_h
