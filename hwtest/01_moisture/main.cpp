// Moisture sensor test: prints the raw ADC value and % every 500 ms.
// Probe in air -> write the raw value to ADC_DRY. Probe in water -> ADC_WET.
#include <Arduino.h>
#include "config.h"
#include "soil_logic.h"

void setup() {
  Serial.begin(115200);
  analogReadResolution(12);
  analogSetAttenuation(ADC_11db);
  delay(1500);
  Serial.println("TEST KELEMBAPAN - GPIO0");
}

void loop() {
  long sum = 0;
  for (int i = 0; i < ADC_SAMPLES; i++) sum += analogRead(PIN_MOISTURE);
  int raw = sum / ADC_SAMPLES;
  Serial.printf("raw=%4d  persen=%5.1f%%  %s\n", raw, moisturePercent(raw, ADC_DRY, ADC_WET),
                raw < 50 || raw > 4050 ? "<- CEK KABEL (nilai di ujung skala)" : "");
  delay(500);
}
