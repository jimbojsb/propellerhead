#ifndef IBUS_PACKET_H
#include <Arduino.h>
#include <StandardCplusplus.h>
#include <vector>
#define IBUS_PACKET_H

typedef std::vector<int> bytes;


class IbusPacket {
  private:
    String intToHexString(int i);  
  public:
    int source;
    int destination;
    int length;
    int checksum;
    bytes message;
    IbusPacket();
    IbusPacket(const IbusPacket& packet);
    IbusPacket(int source, int destination, bytes message);
    IbusPacket(int source, int length, int destination, bytes message, int checksum);
    bool isValid();
    bool isEqualTo(IbusPacket pkt);
    bool messageEquals(bytes message);
    String asString();
    static int calculateChecksum(int source, int length, int destination, bytes message);    
};

#endif
