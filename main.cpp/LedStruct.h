#ifndef _LedStruct_h
#define _LedStruct_h

typedef enum{
  OFF,ON
} LEDStatus;

typedef enum{
  DOWN,UP
} LEDConnectionStatus;

typedef struct LedStruct LedStruct;
struct LedStruct {
  int address;
  char name[30];
  int ledAvailability;
  LEDConnectionStatus ledConnectionStatus;
  LEDStatus r_status;
  LEDStatus g_status;
  LEDStatus b_status;
  int r_intensity;
  int g_intensity;
  int b_intensity;
};

typedef struct ControllerProperties ControllerProperties;
struct ControllerProperties {
  int tagWithTime;
  int overHeatingRecovery;
};

#endif // LedStruct_h
