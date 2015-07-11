#define IBUS_PACKET_DELAY 11

class IbusInterface {
  private:
    unsigned long previousMillis;
    int data[32];
    int index;
    void reset();
    void parsePacket();
  public:
    IbusInterface();  
    void process(unsigned long millis);
};
