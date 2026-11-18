#include <Arduino.h>

#include "config.h"
#include "display.h"
#include "sensors.h"
#include "soil_logic.h"
#include "web.h"

PumpController pump(ON_BELOW, OFF_ABOVE, MAX_ON_MS, LOCKOUT_MS);
Reading lastReading = {0, 0, false, 0};

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
  delay(1500);  // give the USB Serial monitor time to connect, so boot messages are visible
  sensorsBegin();
  displayBegin();
  webBegin();
  lastReadMs = millis() - READ_INTERVAL_MS;  // read immediately
}

static void logState(bool on, uint32_t pauseMs) {
  Serial.printf("raw=%d moisture=%.1f%% temp=%.1fC(%s) pump=%s%s mode=%s\n",
                lastReading.moistureRaw, lastReading.moisture, lastReading.tempC,
                lastReading.tempValid ? "ok" : "--", on ? "ON" : "OFF", pauseMs ? " (jeda resap)" : "",
                pump.mode() == PumpMode::Auto ? "auto" : "manual");
}

void loop() {
  uint32_t now = millis();

  // Sensors are read every READ_INTERVAL_MS.
  bool readDue = now - lastReadMs >= READ_INTERVAL_MS;
  if (readDue) {
    lastReadMs = now;
    lastReading = sensorsRead();
  }

  // The pump is checked every PUMP_TICK_MS, so a 3 s pulse from the web lasts 3 s.
  bool on = pump.update(now, lastReading.moisture);
  setRelay(on);
  bool changed = on != prevPumpOn;
  if (changed) {
    delay(150);          // let the supply settle after the pump switches
    displayRecover();    // the switching spike can garble the LCD
    prevPumpOn = on;
  }

  if (readDue || changed) {
    uint32_t pauseMs = pump.pauseRemainingMs(now);
    displayShow(lastReading, on, pauseMs);
    logState(on, pauseMs);
  }
  delay(PUMP_TICK_MS);
}
