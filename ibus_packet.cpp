#include "ibus_packet.h"
#include "SoftwareSerial.h"

extern SoftwareSerial debug;

IbusPacket::IbusPacket() {
}

IbusPacket::IbusPacket(int source, int length, int destination, int message[]) {
  this->source = source;
  this->destination = destination;
  this->message = message;
  this->length = length;
  this->calculateChecksum();
}

bool IbusPacket::isValid(int checksum) {
  return checksum == this->checksum;
}

bool IbusPacket::messageEquals(int message[]) {
  int messageLength = sizeof(message) / sizeof(int);
  if (this->length - 2 == messageLength) {
    for (int i = 0; i < messageLength; i++) {
      if (this->message[i] != message[i]) {
        return false;
      }
    }   
  }
  return true;
}

void IbusPacket::calculateChecksum() {
  int checksum;
  checksum ^= this->source;
  checksum ^= this->length;
  checksum ^= this->destination;
  for (int i = 0; i < this->length - 2; i++) {
    checksum ^= this->message[i];
  }  
  this->checksum = checksum;
}

