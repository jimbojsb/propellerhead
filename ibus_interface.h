#define IBUS_PACKET_DELAY 11

class IbusInterface {
  private:
    unsigned long previousMillis;
    int data[32];
    int index;
    bool parsePacket();
  public:
    IbusInterface();  
    void process(unsigned long millis);
};
