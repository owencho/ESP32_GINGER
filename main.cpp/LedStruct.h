#ifndef LedStruct_h
#define LedStruct_h

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

#endif // LedStruct_h
