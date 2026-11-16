// Pump test: runs the pump for 3 s once at boot, then stays off.
// Press the board's RESET button to repeat. Put the pump in water before running it.
#include <Arduino.h>
#include "config.h"

void setRelay(bool on) { digitalWrite(PIN_RELAY, (on != RELAY_ACTIVE_LOW) ? HIGH : LOW); }

void setup() {
  setRelay(false);
  pinMode(PIN_RELAY, OUTPUT);
  Serial.begin(115200);
  delay(3000);
  Serial.println("TEST POMPA - menyala 3 detik...");
  setRelay(true);
  delay(3000);
  setRelay(false);
  Serial.println("Pompa OFF. Tekan RESET untuk mengulang.");
}

void loop() {}
