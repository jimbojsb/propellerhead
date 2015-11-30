#include "bluetooth.h"

void Bluetooth::setDeviceName(String name) {
  btModule.stdSetParam("NAME", name); 
  btModule.writeConfig();
}

void Bluetooth::saveAndReset() {
  btModule.writeConfig();
  btModule.reset();
}

void Bluetooth::restoreDefaults() {
  btModule.restore();
  Bluetooth::setDeviceName("Propellerhead");
  Bluetooth::saveAndReset();
}
