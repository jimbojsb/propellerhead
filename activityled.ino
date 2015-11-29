int activityLedState = HIGH;
int activityLedPin = 13;
unsigned long activityLedPreviousMillis = 0;

void activityLedReset(unsigned long millis) {
  if (activityLedState != LOW) {
    if (millis - activityLedPreviousMillis > 20) {
      digitalWrite(activityLedPin, LOW);
      activityLedState = LOW;
    }  
  }
}

void activityLedBlink(unsigned long millis) {
  if (activityLedState == LOW) {
    activityLedPreviousMillis = millis;
    digitalWrite(activityLedPin, HIGH);  
    activityLedState = HIGH;
  }
}
