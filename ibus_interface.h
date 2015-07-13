#include "ibus_packet.h"

class IbusInterface {
  private:
    unsigned long previousMillis;
    int busQuietMillis;
    int data[32];
    int index;
    int packetToSendIndex;
    IbusPacket packetsToSend[4];
    bool parsePacket();
  public:
    IbusInterface();  
    void update(unsigned long millis);
    void send(IbusPacket *packet);
    void sendPendingPackets();
};
