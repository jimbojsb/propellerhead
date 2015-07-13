#include "ibus_packet.h"

class IbusDispatcher {
  private:
    unsigned long previousMillis;
    unsigned long previousCdPlayerAnnounceMillis;
    bool cdPlayerHasBeenPolled;
    
  public:
    IbusDispatcher();
    void dispatch(IbusPacket *packet);
    void update(unsigned long millis);
};
