#pragma once

struct Reading {
  float moisture;  // %
  float tempC;
  bool tempValid;
  float ecUs;      // uS/cm, -1 when no TDS module is fitted (HAS_TDS false)
  int moistureRaw; // raw ADC, used for calibration
};

void sensorsBegin();
Reading sensorsRead();
