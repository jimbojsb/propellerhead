#include "config.h"
#include "debug.h"
#include "SoftwareSerial.h"
#include "ibus.h"
#include "activityled.h"

#ifdef DEBUG_ENABLE
  #define DEBUG_RX 11
  #define DEBUG_TX 10
  SoftwareSerial debug(DEBUG_RX, DEBUG_TX);
#endif

Ibus ibus;

ActivityLed activityLed(13);

void setup() {
  Serial.begin(9600, SERIAL_8E1);
  
  #ifdef DEBUG_ENABLE
    debug.begin(9600);
    debug.println("DEBUG ENABLED");
  #endif
}

void loop() { 
  int currentMillis = millis();
  activityLed.update(currentMillis);
  ibus.process(currentMillis);
}
