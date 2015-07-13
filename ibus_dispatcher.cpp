#include "ibus_dispatcher.h"
#include "ibus_interface.h"
#include "ibus_packet.h"
#include "ibus_devices.h"
#include "arduino.h"
#include "SoftwareSerial.h"
#include "config.h"

extern IbusInterface ibusInterface;

#ifdef DEBUG_ENABLE
  extern SoftwareSerial debug;
#endif

IbusDispatcher::IbusDispatcher() {
  this->cdPlayerHasBeenPolled = false; 
  this->previousCdPlayerAnnounceMillis = 0;
}

void IbusDispatcher::dispatch(IbusPacket *packet) {
  switch (packet->source) {
    case IBUS_RADIO:
      switch(packet->destination) {
        case IBUS_CDPLAYER:
          int pingMessage[1] = {0x01};
          if (packet->messageEquals(pingMessage)) {
            this->cdPlayerHasBeenPolled = true;
            int cdPlayerPongMessage[2] = {0x02, 0x00};
            IbusPacket cdPlayerPong(IBUS_CDPLAYER, 4, IBUS_BROADCAST, cdPlayerPongMessage); 
            ibusInterface.send(&cdPlayerPong);
          }
          break;  
      }
      break;
      
  }
  
}

void IbusDispatcher::update(unsigned long millis) {
  if (!this->cdPlayerHasBeenPolled) {
    if (millis - this->previousCdPlayerAnnounceMillis > 30000) {
      int message[2] = {0x02, 0x01};
      IbusPacket cdplayerAnnounce(IBUS_CDPLAYER, 4, IBUS_BROADCAST, message); 
      ibusInterface.send(&cdplayerAnnounce);
      this->previousCdPlayerAnnounceMillis = millis;
    }
  }  
}
