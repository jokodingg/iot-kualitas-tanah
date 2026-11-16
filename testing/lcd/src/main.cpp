// LCD test: scans the I2C bus, then shows text and a counter.
// Nothing found -> check SDA/SCL. Backlight on but no text -> turn the contrast potentiometer.
#include <Arduino.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>

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

uint8_t addr = 0;
LiquidCrystal_I2C* lcd = nullptr;

void setup() {
  Serial.begin(115200);
  delay(1500);
  Wire.begin(PIN_SDA, PIN_SCL);
  Wire.setClock(50000);  // slower clock is more tolerant of long jumper wires
  Serial.println("TEST LCD - SDA GPIO8, SCL GPIO9. Scan I2C:");
  int found = 0;
  for (uint8_t a = 0x08; a <= 0x77; a++) {  // 0x00-0x07 and 0x78-0x7F are reserved
    Wire.beginTransmission(a);
    if (Wire.endTransmission() == 0) {
      Serial.printf("  ditemukan perangkat di 0x%02X\n", a);
      if (!addr) addr = a;
      found++;
    }
  }
  if (found == 0) {
    Serial.println("  TIDAK ADA perangkat - cek kabel SDA/SCL/VCC/GND");
    return;
  }
  if (found > 3) {
    Serial.println("  JALUR I2C BERMASALAH: terlalu banyak alamat, LCD hanya punya satu.");
    Serial.println("  Cek pull-up 4,7k SDA->3V3 dan SCL->3V3, GND bersama, dan SDA/SCL tidak tertukar.");
    addr = 0;
    return;
  }
  if (addr != LCD_ADDR) Serial.printf("  ganti LCD_ADDR di include/config.h jadi 0x%02X\n", addr);
  lcd = new LiquidCrystal_I2C(addr, 16, 2);
  lcd->init();
  lcd->backlight();
  lcd->print("Tes LCD OK");
}

void loop() {
  if (!lcd) return;
  lcd->setCursor(0, 1);
  lcd->printf("Hitung: %-7lu", millis() / 1000);
  delay(500);
}
