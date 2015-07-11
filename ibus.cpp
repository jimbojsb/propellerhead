#include "ibus.h"
#include "arduino.h"
#include "SoftwareSerial.h"
#include "config.h"
#include "activityled.h"

#ifdef DEBUG_ENABLE
  extern SoftwareSerial debug;
#endif

extern ActivityLed activityLed;

Ibus::Ibus() {
}

void Ibus::process(unsigned long millis) {
  if (Serial.available()) {
    this->incomingData = Serial.read();
    #ifdef DEBUG_ENABLE
      char hexVal[3];
      sprintf(hexVal, "%02x ", this->incomingData);
      debug.print(hexVal);
    #endif
    activityLed.blink(millis);
  }
}

