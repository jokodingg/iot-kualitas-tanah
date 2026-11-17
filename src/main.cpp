#include <Arduino.h>

#include "config.h"
#include "display.h"
#include "sensors.h"
#include "soil_logic.h"
#include "web.h"

PumpController pump(ON_BELOW, OFF_ABOVE, MAX_ON_MS, LOCKOUT_MS);
Reading lastReading = {0, 0, false, 0, 0};

static uint32_t lastReadMs = 0;
static bool prevPumpOn = false;

// 5V active-LOW relay module driven open-drain: ON = OUTPUT LOW, OFF = INPUT
// (released, the module pulls IN up to 5V). A 3.3V HIGH may leave a 5V module half on.
static void setRelay(bool on) {
  if (on) {
    pinMode(PIN_RELAY, OUTPUT);
    digitalWrite(PIN_RELAY, LOW);
  } else {
    pinMode(PIN_RELAY, INPUT);
  }
}

void setup() {
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
  if (on != prevPumpOn) {
    delay(150);          // let the supply settle after the pump switches
    displayRecover();    // the switching spike can garble the LCD
    prevPumpOn = on;
  }
  displayShow(lastReading, on);

  Serial.printf("raw=%d moisture=%.1f%% temp=%.1fC(%s) ec=%.0fuS pump=%s mode=%s\n",
                lastReading.moistureRaw, lastReading.moisture, lastReading.tempC,
                lastReading.tempValid ? "ok" : "--", lastReading.ecUs, on ? "ON" : "OFF",
                pump.mode() == PumpMode::Auto ? "auto" : "manual");
}
