#include "ibus_packet.h"
#include <QueueArray.h>
#include <SoftwareSerial.h>

#define IBUS_DEV_CDPLAYER 0x18
#define IBUS_DEV_RADIO 0x68
#define IBUS_DEV_NAV 0x3B
#define IBUS_DEV_MFW 0x50
#define IBUS_DEV_BOARDMONITOR 0xF0
#define IBUS_DEV_IKE 0x80
#define IBUS_DEV_BROADCAST 0x80

#define IBUS_MSG_CDPLAYER_PING {0x01}
#define IBUS_MSG_CDPLAYER_PONG {0x02, 0x00}

extern SoftwareSerial phControl;

class IbusInterface {
  private:
    bool cdPlayerHasBeenPolled;
    unsigned long currentMillis;
    unsigned long cdPlayerPreviousMillis;
    unsigned long busPreviousMillis;
    unsigned long collisionPreviousMillis;
    int busQuietMillis;
    QueueArray <IbusPacket> *sendQueue;
    QueueArray <IbusPacket> *dispatchQueue;
    bool awaitingSendVerification;
    int receiveQueue[32];
    int receiveQueueLength;
    bool parsePacket();
    void handlePacket(IbusPacket packet);
    void dispatchPackets();
    void writePacket(IbusPacket packet);
    void readIncoming();
    void advertiseCdPlayer();
    void sendPendingPackets();
  public:
    IbusInterface();  
    void update(unsigned long millis);
    void send(IbusPacket packet);
};
