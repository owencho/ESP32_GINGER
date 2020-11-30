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
  String name;
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
  boolean tagWithTime;
  boolean overHeatingRecovery;
};

#endif // LedStruct_h
