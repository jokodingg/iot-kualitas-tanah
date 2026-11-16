// DS18B20 test: counts the sensors on the bus and prints the temperature.
// 0 sensors or -127 -> check the 4.7k pull-up and the yellow wire.
#include <Arduino.h>
#include <DallasTemperature.h>
#include <OneWire.h>

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

OneWire oneWire(PIN_DS18B20);
DallasTemperature ds(&oneWire);

void setup() {
  Serial.begin(115200);
  delay(1500);
  ds.begin();
  Serial.printf("TEST DS18B20 - GPIO3, sensor ditemukan: %d\n", ds.getDeviceCount());
}

void loop() {
  ds.requestTemperatures();
  float t = ds.getTempCByIndex(0);
  if (t == DEVICE_DISCONNECTED_C) Serial.println("TIDAK TERBACA (-127) - cek kabel kuning & pull-up 4,7k");
  else Serial.printf("suhu=%.2f C (pegang probe, angka harus naik)\n", t);
  delay(1000);
}
