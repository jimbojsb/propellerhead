void bluetoothSetDeviceName(String name) {
  btModule.stdSetParam("NAME", name); 
  btModule.writeConfig();
}

void bluetoothSaveAndReset() {
  btModule.writeConfig();
  btModule.reset();
}

void bluetoothRestoreDefaults() {
  btModule.restore();
  bluetoothSetDeviceName("Propellerhead");
  bluetoothSaveAndReset();
}

