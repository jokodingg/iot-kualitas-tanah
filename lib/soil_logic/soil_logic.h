#pragma once
#include <stdint.h>

// Linear map from raw ADC to 0-100 %. A dry probe gives a higher ADC value.
float moisturePercent(int adc, int adcDry, int adcWet);

// DFRobot TDS formula with temperature compensation. Returns EC in uS/cm.
// A temperature outside -20..80 C (e.g. a disconnected DS18B20) falls back to 25 C.
float ecFromVoltage(float volts, float tempC);

enum class PumpMode { Auto, Manual };

// Decides whether the pump runs. Every period on is capped at maxOnMs, in both modes.
// In Auto mode, a lockout of lockoutMs follows each period on.
class PumpController {
 public:
  PumpController(float onBelow, float offAbove, uint32_t maxOnMs, uint32_t lockoutMs);
  bool update(uint32_t nowMs, float moisture);
  void setMode(PumpMode m);
  PumpMode mode() const;
  void manualSet(bool on);
  bool isOn() const;

 private:
  void turnOn(uint32_t nowMs);
  void turnOff(uint32_t nowMs);

  float onBelow_, offAbove_;
  uint32_t maxOnMs_, lockoutMs_;
  PumpMode mode_ = PumpMode::Auto;
  bool on_ = false;
  bool manualRequest_ = false;
  bool hasRun_ = false;
  uint32_t onSince_ = 0;
  uint32_t offSince_ = 0;
};
