// Relay test WITHOUT the pump: the relay must click ON 2 s, OFF 2 s.
// LED and click opposite to the Serial text -> flip RELAY_ACTIVE_LOW in config.h.
#include <Arduino.h>
#include "config.h"

void setRelay(bool on) { digitalWrite(PIN_RELAY, (on != RELAY_ACTIVE_LOW) ? HIGH : LOW); }

void setup() {
  setRelay(false);
  pinMode(PIN_RELAY, OUTPUT);
  Serial.begin(115200);
  delay(1500);
  Serial.printf("TEST RELAY - GPIO4, RELAY_ACTIVE_LOW=%s\n", RELAY_ACTIVE_LOW ? "true" : "false");
}

void loop() {
  setRelay(true);
  Serial.println("relay ON  (LED relay harus menyala, terdengar klik)");
  delay(2000);
  setRelay(false);
  Serial.println("relay OFF (LED relay harus mati)");
  delay(2000);
}
