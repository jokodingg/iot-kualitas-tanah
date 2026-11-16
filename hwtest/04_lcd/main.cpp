// LCD test: scans the I2C bus, then shows text and a counter.
// Nothing found -> check SDA/SCL. Backlight on but no text -> turn the contrast potentiometer.
#include <Arduino.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include "config.h"

uint8_t addr = 0;
LiquidCrystal_I2C* lcd = nullptr;

void setup() {
  Serial.begin(115200);
  delay(1500);
  Wire.begin(PIN_SDA, PIN_SCL);
  Serial.println("TEST LCD - SDA GPIO8, SCL GPIO9. Scan I2C:");
  for (uint8_t a = 1; a < 127; a++) {
    Wire.beginTransmission(a);
    if (Wire.endTransmission() == 0) {
      Serial.printf("  ditemukan perangkat di 0x%02X\n", a);
      if (!addr) addr = a;
    }
  }
  if (!addr) {
    Serial.println("  TIDAK ADA perangkat - cek kabel SDA/SCL/VCC/GND");
    return;
  }
  if (addr != LCD_ADDR) Serial.printf("  ganti LCD_ADDR di config.h jadi 0x%02X\n", addr);
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
