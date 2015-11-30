#ifndef IBUS_PACKET_H
#include <Arduino.h>
#define IBUS_PACKET_H


class IbusPacket {
  private:
    String intToHexString(int i);  
  public:
    int source;
    int destination;
    int length;
    int checksum;
    int *message;
    IbusPacket();
    IbusPacket(int source, int destination, int message[]);
    IbusPacket(int source, int length, int destination, int message[], int checksum);
    bool isValid();
    bool isEqualTo(IbusPacket *pkt);
    bool messageEquals(int message[]);
    String asString();
    static int calculateChecksum(int source, int length, int destination, int message[]);    
};

#endif
