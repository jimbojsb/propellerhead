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
    this->parsePacket();
    this->index++;
    activityLed.blink(millis);
  }
  this->previousMillis = millis;
}

void IbusInterface::parsePacket() {
  debug.println("parse");
  if (this->index >= 4) { // enough bytes to make a packet
    debug.println("long enough");
    int length = this->data[1];
     if (this->index - 1 == length) { // enough bytes to satisfy the hypothetical packet
       debug.println("make packet");
       int source = this->data[0];
       int dest = this->data[2];
       int checksum = this->data[this->index];
       int messageLength = length - 2;
       int message[messageLength];
       for (int i = 0; i < messageLength; i++) {
         message[i] = this->data[3 + i];
       }
       IbusPacket packet(source, length, dest, message);
       debug.println("packet checksum");
       debug.println(packet.checksum, HEX);
       if (packet.isValid(checksum)) {
         debug.println("Valid packet found");  
         this->reset();
       } else {
         debug.println("packet not valid");
       }
     }
  }
}

void IbusInterface::reset() {
    this->index = 0;
    debug.println("reset");
}
