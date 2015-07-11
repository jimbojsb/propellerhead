#include "ibus_packet.h"

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

