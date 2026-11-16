// TDS/EC test: prints the voltage and EC at 25 C.
// Probe in air -> about 0 V. Probe in tap water -> about 100-600 uS/cm.
#include <Arduino.h>
#include "config.h"
#include "soil_logic.h"

void setup() {
  Serial.begin(115200);
  analogReadResolution(12);
  analogSetAttenuation(ADC_11db);
  delay(1500);
  Serial.println("TEST TDS - GPIO1");
}

void loop() {
  long sum = 0;
  for (int i = 0; i < ADC_SAMPLES; i++) sum += analogReadMilliVolts(PIN_TDS);
  float v = sum / (float)ADC_SAMPLES / 1000.0f;
  Serial.printf("tegangan=%.3f V  EC=%.0f uS/cm\n", v, ecFromVoltage(v, 25.0f));
  delay(1000);
}
