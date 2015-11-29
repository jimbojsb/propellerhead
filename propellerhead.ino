#include <EEPROM.h>
#include <SparkFunbc127.h>
#include <SoftwareSerial.h>

SoftwareSerial btControl(2,3);
SoftwareSerial phControl(4,5);
BC127 btModule(&btControl);

void setup() {
  Serial.begin(9600, SERIAL_8E1);
  phControl.begin(9600);
  btControl.begin(9600);
}

void loop() { 
  unsigned long currentMillis = millis();
  activityLedReset(currentMillis);
  ibusUpdate(currentMillis);  
}
