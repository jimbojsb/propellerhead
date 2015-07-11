#include "activityled.h"
#include "arduino.h"

ActivityLed::ActivityLed(int pin) {
  pinMode(pin, OUTPUT);
  digitalWrite(pin, LOW);
  this->pin = pin;
  this->state = LOW;
}

void ActivityLed::update(unsigned long millis) {
  if (this->state != LOW) {
    if (millis - this->previousMillis > 20) {
      digitalWrite(this->pin, LOW);
      this->state = LOW;
    }  
  }
}

void ActivityLed::blink(unsigned long millis) {
  if (this->state == LOW) {
    this->previousMillis = millis;
    digitalWrite(this->pin, HIGH);  
    this->state = HIGH;
  }
}
