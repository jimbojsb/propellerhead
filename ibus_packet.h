#ifndef IBUS_PACKET_H
#define IBUS_PACKET_H

class IbusPacket {
  private:
    void calculateChecksum();
    
  public:
    int source;
    int destination;
    int length;
    int checksum;
    int *message;
    
    IbusPacket(int source, int length, int destination, int message[]);
    bool isValid(int checksum);
    
};

#endif
