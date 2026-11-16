// TDS/EC test: prints the voltage and EC at 25 C.
// Probe in air -> about 0 V. Probe in tap water -> about 100-600 uS/cm.
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
  Serial.println("TEST TDS - GPIO1");
}

void loop() {
  long sum = 0;
  for (int i = 0; i < ADC_SAMPLES; i++) sum += analogReadMilliVolts(PIN_TDS);
  float v = sum / (float)ADC_SAMPLES / 1000.0f;
  Serial.printf("tegangan=%.3f V  EC=%.0f uS/cm\n", v, ecFromVoltage(v, 25.0f));
  delay(1000);
}
