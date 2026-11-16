#pragma once

struct Reading {
  float moisture;  // %
  float tempC;
  bool tempValid;
  float ecUs;      // uS/cm
  int moistureRaw; // raw ADC, used for calibration
};

void sensorsBegin();
Reading sensorsRead();
