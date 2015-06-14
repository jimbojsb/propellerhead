#define IBUS_PACKET_DELAY 11

class Ibus {
  private:
    int previousMillis;
    int incomingData;
  
  public:
    Ibus();  
    void init();
    void process(int millis);
};
