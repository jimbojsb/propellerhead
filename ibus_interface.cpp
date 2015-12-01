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
    this->collisionPreviousMillis = 0;
    this->receiveQueueLength = 0;
    this->awaitingSendVerification = false;
    this->sendQueue = new QueueArray <IbusPacket>;
    this->dispatchQueue = new QueueArray <IbusPacket>;
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
      phControl.println(F("advertising cd player"));
      std::vector<int> msg = IBUS_MSG_CDPLAYER_PONG;
      IbusPacket pkt = IbusPacket(IBUS_DEV_CDPLAYER, IBUS_DEV_BROADCAST, msg);
      this->send(pkt);
      this->cdPlayerPreviousMillis = this->currentMillis;
    }
  }
}

void IbusInterface::send(IbusPacket packet) {
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
    if (this->awaitingSendVerification && this->currentMillis - this->collisionPreviousMillis > 200) {
      phControl.println(F("resending missing packets"));
      this->collisionPreviousMillis = this->currentMillis;
    } else if (this->awaitingSendVerification) {
      return; // don't try to resend, hasn't been long enough
    } else {
      phControl.println(F("sending packets"));
      this->awaitingSendVerification = true;
    }
    IbusPacket pkt = this->sendQueue->front();
    phControl.println(pkt.asString());
    this->writePacket(pkt);
  }
}

void IbusInterface::dispatchPackets() {
  if (!this->dispatchQueue->isEmpty()) {
    IbusPacket pkt = this->dispatchQueue->pop();
    phControl.println(F("dispatching packets"));
    // we should receive back an echo of our sent packets. Use this for 
    // confirmation that the send worked.
    if (this->awaitingSendVerification) {
      IbusPacket packetToVerify = this->sendQueue->front();
      if (pkt.isEqualTo(packetToVerify)) {
        this->awaitingSendVerification = false;
        this->sendQueue->pop();
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
       bytes message;
       for (int i = 0; i < messageLength; i++) {
         message.push_back(this->receiveQueue[3 + i]);
       }
       IbusPacket packet(source, length, dest, message, checksum);
       if (packet.isValid()) {  
         this->receiveQueueLength = 0;
         this->dispatchQueue->push(packet);
         phControl.println("< " + packet.asString());
         return true;
       } 
    }
  }
  return false;
}

void IbusInterface::handlePacket(IbusPacket packet) {
  phControl.println(F("handling packet"));
  switch (packet.source) {
    case IBUS_DEV_RADIO:
      switch(packet.destination) {
        case IBUS_DEV_CDPLAYER:
          bytes msg;
          msg.push_back(0x01);
          if (packet.messageEquals(msg)) {
            phControl.println(F("detected cdplayer ping"));
            this->cdPlayerHasBeenPolled = true;
            this->advertiseCdPlayer();
          }
          break;  
      }
      break;
    default:
      phControl.println(F("no packet rule"));
      phControl.println(packet.asString());
  }
}

void IbusInterface::writePacket(IbusPacket packet) {
    phControl.println("writing packet");
    Serial.write(packet.source);
    Serial.write(packet.length);
    Serial.write(packet.destination);
    for (int i = 0; i < packet.length - 2; i++) {
      Serial.write(packet.message[i]);  
    }
    Serial.write(packet.checksum);
}
