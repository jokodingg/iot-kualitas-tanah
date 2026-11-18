#pragma once
#include <stdint.h>

// Linear map from raw ADC to 0-100 %. A dry probe gives a higher ADC value.
float moisturePercent(int adc, int adcDry, int adcWet);


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
  // Run the pump for durationMs (capped at maxOnMs) in either mode, e.g. from a web button.
  // Moisture and the Auto pause are ignored during the pulse; no pause follows it.
  void pulse(uint32_t nowMs, uint32_t durationMs);
  // Turn the pump off now and cancel any pulse or manual request.
  void stop(uint32_t nowMs);
  // Milliseconds left in the Auto-mode pause after a watering pulse; 0 when not pausing.
  uint32_t pauseRemainingMs(uint32_t nowMs) const;

 private:
  void turnOn(uint32_t nowMs);
  void turnOff(uint32_t nowMs);

  float onBelow_, offAbove_;
  uint32_t maxOnMs_, lockoutMs_;
  PumpMode mode_ = PumpMode::Auto;
  bool on_ = false;
  bool manualRequest_ = false;
  bool hasRun_ = false;
  bool pulsing_ = false;
  uint32_t pulseMs_ = 0;
  uint32_t onSince_ = 0;
  uint32_t offSince_ = 0;
};
