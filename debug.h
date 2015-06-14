//------------------
#define DEBUG_ENABLE
//------------------

#include "SoftwareSerial.h"

#ifdef DEBUG_ENABLE
  #define DEBUG_RX 11
  #define DEBUG_TX 10
  extern SoftwareSerial debug;
#endif
