#include <SoftwareSerial.h>
#include <SparkFunbc127.h>
#include "ibus_interface.h"
#include "activityled.h"

SoftwareSerial btControl(2,3);
SoftwareSerial phControl(4,5);
BC127 btModule(&btControl);

IbusInterface ibusInterface;

ActivityLed activityLed(13);

void setup() {
  Serial.begin(9600, SERIAL_8E1);
  phControl.begin(9600);
  btControl.begin(9600);

  phControl.println("booted");
}

void loop() { 
  unsigned long currentMillis = millis();
  activityLed.update(currentMillis);
  ibusInterface.update(currentMillis);
}
