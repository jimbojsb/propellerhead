#include "debug.h"
#include "ibus.h"
#include "arduino.h"

Ibus::Ibus() {
}

void Ibus::init() {
  Serial.begin(9600, SERIAL_8E1);
}

void Ibus::process(int millis) {
  if (Serial.available()) {
    this->incomingData = Serial.read();
    #ifdef DEBUG_ENABLE
      debug.println(this->incomingData, HEX);
    #endif
  }
}

