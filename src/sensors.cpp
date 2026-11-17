#include "sensors.h"

#include <Arduino.h>
#include <DallasTemperature.h>
#include <OneWire.h>

#include "config.h"
#include "soil_logic.h"

static OneWire oneWire(PIN_DS18B20);
static DallasTemperature ds(&oneWire);
static float lastGoodTemp = 0;
static int badTempReads = TEMP_HOLD_READS;  // no good value yet

static int averageRaw(int pin) {
  long sum = 0;
  for (int i = 0; i < ADC_SAMPLES; i++) sum += analogRead(pin);
  return sum / ADC_SAMPLES;
}

static float averageVolts(int pin) {
  long sum = 0;
  for (int i = 0; i < ADC_SAMPLES; i++) sum += analogReadMilliVolts(pin);
  return (sum / (float)ADC_SAMPLES) / 1000.0f;
}

void sensorsBegin() {
  analogReadResolution(12);
  analogSetAttenuation(ADC_11db);
  ds.begin();
  ds.setWaitForConversion(false);  // do not block loop() for 750 ms
  ds.requestTemperatures();
}

Reading sensorsRead() {
  Reading r;
  float t = ds.getTempCByIndex(0);  // result of the previous request
  ds.requestTemperatures();
  bool ok = t != DEVICE_DISCONNECTED_C && t > -20.0f && t < 80.0f;
  if (ok) {
    lastGoodTemp = t;
    badTempReads = 0;
  } else if (badTempReads < TEMP_HOLD_READS) {
    badTempReads++;
  }
  // A single failed read (often a pump spike) keeps the last good value.
  r.tempValid = badTempReads < TEMP_HOLD_READS;
  r.tempC = r.tempValid ? lastGoodTemp : t;
  t = r.tempC;

  r.moistureRaw = averageRaw(PIN_MOISTURE);
  r.moisture = moisturePercent(r.moistureRaw, ADC_DRY, ADC_WET);
  r.ecUs = ecFromVoltage(averageVolts(PIN_TDS), r.tempValid ? t : 25.0f);
  return r;
}
