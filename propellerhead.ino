#include "config.h"
#include "SoftwareSerial.h"
#include "ibus_interface.h"
#include "activityled.h"
#include "ibus_dispatcher.h"

#ifdef DEBUG_ENABLE
  #define DEBUG_RX 11
  #define DEBUG_TX 10
  SoftwareSerial debug(DEBUG_RX, DEBUG_TX);
#endif

IbusInterface ibusInterface;
IbusDispatcher ibusDispatcher;

ActivityLed activityLed(13);

void setup() {
  Serial.begin(9600, SERIAL_8E1);
  
  #ifdef DEBUG_ENABLE
    debug.begin(9600);
  #endif
}

void loop() { 
  int currentMillis = millis();
  activityLed.update(currentMillis);
  ibusInterface.update(currentMillis);
  ibusDispatcher.update(currentMillis);
}
