#include "ibus_packet.h"

IbusPacket::IbusPacket() {
}

IbusPacket::IbusPacket(int source, int destination, std::vector<int> message) {
  this->source = source;
  this->destination = destination;
  this->message = message;
  this->length = sizeof(message) / sizeof(message[0]);
  this->checksum = IbusPacket::calculateChecksum(source, length, destination, message);
}

IbusPacket::IbusPacket(int source, int length, int destination, std::vector<int> message, int checksum) {
  this->source = source;
  this->destination = destination;
  this->message = message;
  this->length = length;
  this->checksum = checksum;
}

IbusPacket::IbusPacket(const IbusPacket& packet) {
  this->source = packet.source;
  this->destination = packet.destination;
  this->message = packet.message;
  this->length = packet.length;
  this->checksum = packet.checksum;
}

bool IbusPacket::isValid() {
  return this->checksum == IbusPacket::calculateChecksum(this->source, this->length, this->destination, this->message);
}

bool IbusPacket::messageEquals(bytes message) {
  int messageLength = message.size();
  if (this->length - 2 == messageLength) {
    for (int i = 0; i < messageLength; i++) {
      if (this->message[i] != message[i]) {
        return false;
      }
    }   
  }
  return true;
}

bool IbusPacket::isEqualTo(IbusPacket pkt) {
  bool identical = (
    this->source == pkt.source &&
    this->length == pkt.length &&
    this->destination == pkt.destination &&
    this->checksum == pkt.checksum
  );
  return identical;
}

int IbusPacket::calculateChecksum(int source, int length, int destination, bytes message) {
  int checksum;
  checksum ^= source;
  checksum ^= length;
  checksum ^= destination;
  for (int i = 0; i < length - 2; i++) {
    checksum ^= message[i];
  }  
  return checksum;
}

String IbusPacket::asString() {
  String output = "";
  output += this->intToHexString(this->source) + " ";
  output += this->intToHexString(this->length) + " ";
  output += this->intToHexString(this->destination) + " ";
  int messageLength = this->length - 2;
  for (int i = 0; i < messageLength; i++) {
    output += this->intToHexString(this->message[i]) + " ";
  }
  output += this->intToHexString(this->checksum) + " ";
  return output;
}

String IbusPacket::intToHexString(int i) {
  String retval(i, HEX);
  if (i < 10) {
    retval = "0" + retval;
  }
  return retval;
}

