#include "ibus_interface.h"
#include "arduino.h"
#include "activityled.h"
#include "ibus_packet.h"

extern ActivityLed activityLed;

IbusInterface::IbusInterface() {
    this->cdPlayerHasBeenPolled = false;
    this->cdPlayerPreviousMillis = 0;
    this->busPreviousMillis = 0;
    this->busQuietMillis = 0;
    this->currentMillis = 0;
    this->receiveQueueLength = 0;
    this->awaitingSendVerification = false;
    this->sendQueue = new QueueArray <IbusPacket *>;
    this->dispatchQueue = new QueueArray <IbusPacket *>;
}

void IbusInterface::readIncoming() {
  if (Serial.available()) {
    int incoming = Serial.read();
    this->receiveQueue[this->receiveQueueLength] = incoming;
    this->receiveQueueLength++;
    bool foundPacket = this->parsePacket();
    if (foundPacket) {
        activityLed.blink(this->currentMillis);
    }
    this->busQuietMillis = 0;
  } else {
    this->busQuietMillis += this->currentMillis - this->busPreviousMillis;
  }
  this->busPreviousMillis = this->currentMillis;;
}

void IbusInterface::advertiseCdPlayer() {
  if (this->cdPlayerHasBeenPolled) {
    if (this->currentMillis - this->cdPlayerPreviousMillis >= 30000 || this->cdPlayerPreviousMillis == 0) {
      int msg[2] = IBUS_MSG_CDPLAYER_PONG;
      IbusPacket pkt = IbusPacket(IBUS_DEV_CDPLAYER, IBUS_DEV_BROADCAST, msg);
      this->send(&pkt);
      this->cdPlayerPreviousMillis = this->currentMillis;
    }
  }
}

void IbusInterface::send(IbusPacket *packet) {
  this->sendQueue->push(packet);
}

void IbusInterface::update(unsigned long millis) { 
  this->currentMillis = millis;
  this->readIncoming();
  this->advertiseCdPlayer();
  this->dispatchPackets();
  this->sendPendingPackets();
}

void IbusInterface::sendPendingPackets() {
  if (!this->sendQueue->isEmpty() && this->busQuietMillis >= 11) {
    IbusPacket *pkt = this->sendQueue->front();
    phControl.println("writing packet");
    phControl.println(pkt->source);
    return;
    this->writePacket(pkt);
    this->awaitingSendVerification = true;
  }
}

void IbusInterface::dispatchPackets() {
  if (!this->dispatchQueue->isEmpty()) {
    IbusPacket *pkt = this->dispatchQueue->pop();
    // we should receive back an echo of our sent packets. Use this for 
    // confirmation that the send worked.
    if (this->awaitingSendVerification) {
      IbusPacket *packetToVerify = this->sendQueue->front();
      if (pkt->isEqualTo(packetToVerify)) {
        this->awaitingSendVerification = false;
        this->sendQueue->pop();
        delete packetToVerify;
      }
    } else { // this is a new packet, not an echo of one we sent
      this->handlePacket(pkt);
    }
  }
}

bool IbusInterface::parsePacket() {
  if (this->receiveQueueLength >= 5) { // enough bytes to make a packet
    int length = this->receiveQueue[1];
     if (this->receiveQueueLength - 2 >= length) { // enough bytes to satisfy the hypothetical packet
       int source = this->receiveQueue[0];
       int dest = this->receiveQueue[2];
       int checksum = this->receiveQueue[this->receiveQueueLength - 1];
       int messageLength = length - 2;
       int message[messageLength];
       for (int i = 0; i < messageLength; i++) {
         message[i] = this->receiveQueue[3 + i];
       }
       IbusPacket packet(source, length, dest, message, checksum);
       if (packet.isValid()) {  
         this->receiveQueueLength = 0;
         this->dispatchQueue->push(&packet);
         phControl.println("< " + packet.asString());
         return true;
       } 
    }
  }
  return false;
}

void IbusInterface::handlePacket(IbusPacket *packet) {
  switch (packet->source) {
    case IBUS_DEV_RADIO:
      switch(packet->destination) {
        case IBUS_DEV_CDPLAYER:
          int msg[1] = {0x01};
          if (packet->messageEquals(msg)) {
            this->cdPlayerHasBeenPolled = true;
            this->advertiseCdPlayer();
          }
          break;  
      }
      break;
  }
  delete packet;
}

void IbusInterface::writePacket(IbusPacket *packet) {
    Serial.write(packet->source);
    Serial.write(packet->length);
    Serial.write(packet->destination);
    for (int i = 0; i < packet->length - 2; i++) {
      Serial.write(packet->message[i]);  
    }
    Serial.write(packet->checksum);
}
