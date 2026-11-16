#include "soil_logic.h"

float moisturePercent(int adc, int adcDry, int adcWet) {
  if (adcDry == adcWet) return 0.0f;
  float pct = 100.0f * (float)(adcDry - adc) / (float)(adcDry - adcWet);
  if (pct < 0.0f) return 0.0f;
  if (pct > 100.0f) return 100.0f;
  return pct;
}

float ecFromVoltage(float volts, float tempC) {
  float t = (tempC < -20.0f || tempC > 80.0f) ? 25.0f : tempC;
  float comp = 1.0f + 0.02f * (t - 25.0f);
  float v = volts / comp;
  float tds = (133.42f * v * v * v - 255.86f * v * v + 857.39f * v) * 0.5f;  // ppm
  return tds * 2.0f;
}

PumpController::PumpController(float onBelow, float offAbove, uint32_t maxOnMs, uint32_t lockoutMs)
    : onBelow_(onBelow), offAbove_(offAbove), maxOnMs_(maxOnMs), lockoutMs_(lockoutMs) {}

void PumpController::turnOn(uint32_t nowMs) {
  on_ = true;
  onSince_ = nowMs;
}

void PumpController::turnOff(uint32_t nowMs) {
  on_ = false;
  hasRun_ = true;
  offSince_ = nowMs;
}

bool PumpController::update(uint32_t nowMs, float moisture) {
  if (on_ && nowMs - onSince_ >= maxOnMs_) {
    turnOff(nowMs);
    manualRequest_ = false;  // a manual run must be requested again
    return on_;
  }

  if (mode_ == PumpMode::Manual) {
    if (manualRequest_ && !on_) turnOn(nowMs);
    else if (!manualRequest_ && on_) turnOff(nowMs);
    return on_;
  }

  if (on_) {
    if (moisture > offAbove_) turnOff(nowMs);
  } else {
    bool locked = hasRun_ && nowMs - offSince_ < lockoutMs_;
    if (moisture < onBelow_ && !locked) turnOn(nowMs);
  }
  return on_;
}

void PumpController::setMode(PumpMode m) {
  if (m == mode_) return;
  mode_ = m;
  manualRequest_ = false;
}

PumpMode PumpController::mode() const { return mode_; }

void PumpController::manualSet(bool on) { manualRequest_ = on; }

bool PumpController::isOn() const { return on_; }
