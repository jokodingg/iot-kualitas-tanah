// Relay test WITHOUT the pump: the relay must click ON 2 s, OFF 2 s.
// LED and click opposite to the Serial text -> check the IN wire and the module's VCC (5V).
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

// 5V active-LOW relay module driven open-drain from a 3.3V pin:
//   ON  = pin OUTPUT LOW
//   OFF = pin INPUT (released), so the module pulls IN up to 5V itself.
// Driving 3.3V HIGH may leave a 5V module half on.
void setRelay(bool on) {
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
  delay(1500);
  Serial.printf("TEST RELAY - GPIO4, open-drain, aktif LOW\n");
}

void loop() {
  setRelay(true);
  Serial.println("relay ON  (LED relay harus menyala, terdengar klik)");
  delay(2000);
  setRelay(false);
  Serial.println("relay OFF (LED relay harus mati)");
  delay(2000);
}
