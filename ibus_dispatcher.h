#include "ibus_packet.h"

class IbusDispatcher {
  private:
    unsigned long previousMillis;
  public:
    IbusDispatcher();
    void dispatch(unsigned long millis, IbusPacket *packet);
};
