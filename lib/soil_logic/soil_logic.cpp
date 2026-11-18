#include "soil_logic.h"

float moisturePercent(int adc, int adcDry, int adcWet) {
  if (adcDry == adcWet) return 0.0f;
  float pct = 100.0f * (float)(adcDry - adc) / (float)(adcDry - adcWet);
  if (pct < 0.0f) return 0.0f;
  if (pct > 100.0f) return 100.0f;
  return pct;
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
  if (pulsing_) {
    if (nowMs - onSince_ >= pulseMs_) {
      on_ = false;  // no auto pause after a pulse
      pulsing_ = false;
    }
    return on_;
  }

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

uint32_t PumpController::pauseRemainingMs(uint32_t nowMs) const {
  if (mode_ != PumpMode::Auto || on_ || !hasRun_) return 0;
  uint32_t elapsed = nowMs - offSince_;
  return elapsed < lockoutMs_ ? lockoutMs_ - elapsed : 0;
}

void PumpController::pulse(uint32_t nowMs, uint32_t durationMs) {
  pulsing_ = true;
  pulseMs_ = durationMs < maxOnMs_ ? durationMs : maxOnMs_;
  manualRequest_ = false;
  on_ = true;
  onSince_ = nowMs;
}

void PumpController::stop(uint32_t nowMs) {
  (void)nowMs;
  pulsing_ = false;
  manualRequest_ = false;
  on_ = false;
}
