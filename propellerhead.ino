#include "debug.h"
#include "SoftwareSerial.h"
#include "ibus.h"
Ibus ibus;

#ifdef DEBUG_ENABLE
  SoftwareSerial debug(DEBUG_RX, DEBUG_TX);
#endif

void setup() {
  ibus.init();
  #ifdef DEBUG_ENABLE
    debug.begin(9600);
    debug.println("DEBUG ENABLED");
  #endif
}

void loop() { 
  int currentMillis = millis();
  ibus.process(currentMillis);
}
