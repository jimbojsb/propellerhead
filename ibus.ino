#define IBUS_DEV_CDPLAYER 0x18
#define IBUS_DEV_RADIO 0x68
#define IBUS_DEV_NAV 0x3B
#define IBUS_DEV_MFW 0x50
#define IBUS_DEV_BOARDMONITOR 0xF0
#define IBUS_DEV_IKE 0x80
#define IBUS_DEV_BROADCAST 0x80

#define IBUS_MSG_CDPLAYER_PING (int[]) {0x01}
#define IBUS_MSG_CDPLAYER_PONG (int[]) {0x02, 0x00}

struct IbusPacket {
  int source;
  int destination;
  int length;
  int checksum;
  int message[];
};

struct IbusPacketQueue {
  int length = 0;
  IbusPacket packets[8];
} ibusSendQueue, ibusReceiveQueue;

struct IbusByteQueue {
  int length = 0;
  int data[32];
} ibusIncomingBytes;

bool ibusCdPlayerHasBeenPolled = false;
unsigned long ibusPreviousCdPlayerAnnounceMillis;
unsigned long ibusPreviousMillis;
int ibusCurrentByteIndex = 0;
int ibusSendQueueIndex = 0;
int ibusQuietMillis = 0;

bool ibusPacketHasMessage(struct IbusPacket *packet, int message[]) {
  int messageLength = sizeof(message) / sizeof(int);
  if (packet->length - 2 == messageLength) {
    for (int i = 0; i < messageLength; i++) {
      if (packet->message[i] != message[i]) {
        return false;
      }
    }  
    return true; 
  }
  return false;
}

int ibusPacketCalculateChecksum(int source, int length, int destination, int message[]) {
  int checksum;
  checksum ^= source;
  checksum ^= length;
  checksum ^= destination;
  for (int i = 0; i < length - 2; i++) {
    checksum ^= message[i];
  }  
  return checksum;
}

IbusPacket* ibusCreatePacket(int source, int dest, int message[]) {
  IbusPacket p;
  p.source = source;
  p.destination = dest;
  p.message = memcpy(message);
  p.length = sizeof(message) / sizeof(message[0]);
  p.checksum = ibusPacketCalculateChecksum(p.source, p.length, p.destination, p.message);
  return p;
}

void ibusWritePacket(struct IbusPacket *packet) {
  Serial.write(packet.source);
  Serial.write(packet.length);
  Serial.write(packet.destination);
  for (int i = 0; i < packet.length - 2; i++) {
    Serial.write(packet.message[i]);  
  }
  Serial.write(packet.checksum);
}

void ibusSendPacket(struct IbusPacket packet) {
    ibusSendQueue.packets[ibusSendQueue.length] = packet;
    ibusSendQueue.length++;
}

void ibusHandlePacket(IbusPacket packet) {
  switch (packet.source) {
    case IBUS_DEV_RADIO:
      switch(packet.destination) {
        case IBUS_DEV_CDPLAYER:
          int msg[1] = {0x01};
          bool isPing = ibusPacketHasMessage(packet, &msg);
          if (isPing) {
            ibusCdPlayerHasBeenPolled = true;
            int msg[2] = {0x02, 0x00};
            ibusSendPacket(ibusCreatePacket(IBUS_DEV_CDPLAYER, IBUS_DEV_BROADCAST, msg));
          }
          break;  
      }
      break;
      
  }
}

void ibusDispatchPackets() {
  
}

void ibusUpdate(unsigned long millis) {
  if (ibusCdPlayerHasBeenPolled) {
    if (millis - ibusPreviousCdPlayerAnnounceMillis > 30000 || ibusPreviousCdPlayerAnnounceMillis == 0) {
      int msg[2] = {0x02, 0x00};
      IbusPacket p = ibusCreatePacket(IBUS_DEV_CDPLAYER, IBUS_DEV_BROADCAST, msg);
      ibusSendPacket(p);
      ibusPreviousCdPlayerAnnounceMillis = millis;
    }
  }  
  ibusReadIncoming(millis);
  ibusDispatchPackets();
}

void ibusReadIncoming(unsigned long millis) { 
  if (Serial.available()) {
    int incoming = Serial.read();
    ibusIncomingBytes.data[ibusIncomingBytes.length] = incoming;
    ibusIncomingBytes.length++;
    bool foundPacket = ibusParsePacket();
    if (foundPacket) {
        activityLedBlink(millis);
    }
    ibusQuietMillis = 0;
  } else {
    ibusQuietMillis += millis - ibusPreviousMillis;
  }
  ibusPreviousMillis = millis;
}

bool ibusParsePacket() {
  if (ibusIncomingBytes.length >= 4) { // enough bytes to make a packet
    int length = ibusIncomingBytes.data[1];
     if (ibusIncomingBytes.length - 1 >= length) { // enough bytes to satisfy the hypothetical packet
       int source = ibusIncomingBytes.data[0];
       int destination = ibusIncomingBytes.data[2];
       int checksum = ibusIncomingBytes.data[ibusIncomingBytes.length - 1];
       int messageLength = length - 2;
       int message[messageLength];
       for (int i = 0; i < messageLength; i++) {
         message[i] = ibusIncomingBytes.data[3 + i];
       }
       if (ibusPacketCalculateChecksum(source, length, destination, message) == checksum) {
         ibusIncomingBytes.length = 0;
         ibusReceiveQueue.packets[ibusReceiveQueue.length] = ibusCreatePacket(source, destination, message);
         return true;
       }
     }
  }
  return false;
}
