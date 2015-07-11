#define IBUS_PACKET_DELAY 11

class Ibus {
  private:
    unsigned long previousMillis;
    int incomingData;
  
  public:
    Ibus();  
    void process(unsigned long millis);
};
