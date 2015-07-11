#include "ibus_interface.h"
#include "arduino.h"
#include "SoftwareSerial.h"
#include "config.h"
#include "activityled.h"
#include "ibus_packet.h"
#include "ibus_dispatcher.h"

#ifdef DEBUG_ENABLE
  extern SoftwareSerial debug;
#endif

extern ActivityLed activityLed;

IbusInterface::IbusInterface() {
  this->index = 0;
}

void IbusInterface::process(unsigned long millis) { 
  if (Serial.available()) {
    int incoming = Serial.read();
    this->data[this->index] = incoming;
    debug.println(incoming, HEX);
    bool foundPacket = this->parsePacket();
    if (foundPacket) {
        activityLed.blink(millis);
    } else {
      this->index++;
    }
  }
  this->previousMillis = millis;
}

bool IbusInterface::parsePacket() {
  if (this->index >= 4) { // enough bytes to make a packet
    int length = this->data[1];
     if (this->index - 1 == length) { // enough bytes to satisfy the hypothetical packet
       int source = this->data[0];
       int dest = this->data[2];
       int checksum = this->data[this->index];
       int messageLength = length - 2;
       int message[messageLength];
       for (int i = 0; i < messageLength; i++) {
         message[i] = this->data[3 + i];
       }
       IbusPacket packet(source, length, dest, message);
       if (packet.isValid(checksum)) {
         debug.println("Valid packet found");  
         this->index = 0;
         return true;
       } else {
         debug.println("packet not valid");
       }
     }
  }
  return false;
}
