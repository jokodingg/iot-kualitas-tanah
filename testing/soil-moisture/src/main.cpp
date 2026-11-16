// Moisture sensor test: prints the raw ADC value and % every 500 ms.
// Probe in air -> write the raw value to ADC_DRY. Probe in water -> ADC_WET.
#include <Arduino.h>

// Pins and settings, copied from include/config.h in the main project.
const int PIN_MOISTURE = 0;
const int PIN_TDS = 1;
const int PIN_DS18B20 = 3;
const int PIN_RELAY = 4;
const int PIN_SDA = 8;
const int PIN_SCL = 9;
const uint8_t LCD_ADDR = 0x27;
const int ADC_SAMPLES = 10;
const int ADC_DRY = 3000;
const int ADC_WET = 1300;

float moisturePercent(int adc, int adcDry, int adcWet) {
  float pct = 100.0f * (adcDry - adc) / (float)(adcDry - adcWet);
  return pct < 0 ? 0 : (pct > 100 ? 100 : pct);
}

float ecFromVoltage(float volts, float tempC) {
  float v = volts / (1.0f + 0.02f * (tempC - 25.0f));
  return (133.42f * v * v * v - 255.86f * v * v + 857.39f * v) * 0.5f * 2.0f;
}

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
