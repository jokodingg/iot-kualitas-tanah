#include <Arduino.h>

#include "config.h"
#include "display.h"
#include "sensors.h"
#include "soil_logic.h"
#include "web.h"

PumpController pump(ON_BELOW, OFF_ABOVE, MAX_ON_MS, LOCKOUT_MS);
Reading lastReading = {0, 0, false, 0, 0};

static uint32_t lastReadMs = 0;

static void setRelay(bool on) {
  digitalWrite(PIN_RELAY, (on != RELAY_ACTIVE_LOW) ? HIGH : LOW);
}

void setup() {
  pinMode(PIN_RELAY, OUTPUT);
  setRelay(false);
  Serial.begin(115200);
  sensorsBegin();
  displayBegin();
  webBegin();
  lastReadMs = millis() - READ_INTERVAL_MS;  // read immediately
}

void loop() {
  uint32_t now = millis();
  if (now - lastReadMs < READ_INTERVAL_MS) return;
  lastReadMs = now;

  lastReading = sensorsRead();
  bool on = pump.update(now, lastReading.moisture);
  setRelay(on);
  displayShow(lastReading, on);

  Serial.printf("raw=%d moisture=%.1f%% temp=%.1fC(%s) ec=%.0fuS pump=%s mode=%s\n",
                lastReading.moistureRaw, lastReading.moisture, lastReading.tempC,
                lastReading.tempValid ? "ok" : "--", lastReading.ecUs, on ? "ON" : "OFF",
                pump.mode() == PumpMode::Auto ? "auto" : "manual");
}
