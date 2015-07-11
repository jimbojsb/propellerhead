class ActivityLed {
  private:
    unsigned long previousMillis;
    int pin;
    int state;
  public:
    ActivityLed(int pin);
    void update(unsigned long millis);
    void blink(unsigned long millis);
};
