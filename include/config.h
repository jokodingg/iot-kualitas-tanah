#pragma once
#include <stdint.h>

// Pins (ESP32-C3 Super Mini)
constexpr int PIN_MOISTURE = 0;  // ADC
constexpr int PIN_TDS = 1;       // ADC
constexpr int PIN_DS18B20 = 3;
constexpr int PIN_RELAY = 4;
constexpr int PIN_SDA = 8;
constexpr int PIN_SCL = 9;

// Set true if the relay module switches ON when its input is LOW.
constexpr bool RELAY_ACTIVE_LOW = true;  // most cheap opto relay modules; check yours

constexpr uint8_t LCD_ADDR = 0x27;  // try 0x3F if the screen stays blank

// Watering rules
constexpr float ON_BELOW = 35.0f;    // % moisture
constexpr float OFF_ABOVE = 55.0f;   // % moisture
constexpr uint32_t MAX_ON_MS = 10000;
constexpr uint32_t LOCKOUT_MS = 60000;

constexpr uint32_t READ_INTERVAL_MS = 2000;
constexpr int ADC_SAMPLES = 10;

// Moisture calibration: raw ADC with the probe in air (dry) and in water (wet).
// Placeholder values; measure on your own sensor.
constexpr int ADC_DRY = 3000;
constexpr int ADC_WET = 1300;

constexpr const char* AP_SSID = "KualitasTanah";
constexpr const char* AP_PASS = "tanah123";
