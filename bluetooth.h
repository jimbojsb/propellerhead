#include <Arduino.h>
#include <SparkFunbc127.h>

extern BC127 btModule;

class Bluetooth {
  public:
    static void setDeviceName(String name);
    static void saveAndReset();
    static void restoreDefaults();
};
