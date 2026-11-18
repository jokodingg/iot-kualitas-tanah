#include "display.h"

#include <LiquidCrystal_I2C.h>
#include <Wire.h>

#include "config.h"

static LiquidCrystal_I2C lcd(LCD_ADDR, 16, 2);

static void printLine(int row, const char* text) {
  char buf[17];
  snprintf(buf, sizeof(buf), "%-16s", text);  // pad so old characters get overwritten
  lcd.setCursor(0, row);
  lcd.print(buf);
}

void displayBegin() {
  Wire.begin(PIN_SDA, PIN_SCL);
  lcd.init();
  lcd.backlight();
  printLine(0, "Kualitas Tanah");
  printLine(1, "Memulai...");
}

void displayRecover() {
  lcd.init();
  lcd.backlight();
}

void displayShow(const Reading& r, bool pumpOn) {
  char line[24];
  if (r.tempValid) snprintf(line, sizeof(line), "M:%d%% T:%.1fC", (int)(r.moisture + 0.5f), r.tempC);
  else snprintf(line, sizeof(line), "M:%d%% T:--", (int)(r.moisture + 0.5f));
  printLine(0, line);
  snprintf(line, sizeof(line), "Pompa: %s", pumpOn ? "ON" : "OFF");
  printLine(1, line);
}
