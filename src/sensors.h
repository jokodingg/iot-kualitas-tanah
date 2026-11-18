#pragma once

struct Reading {
  float moisture;  // %
  float tempC;
  bool tempValid;
  int moistureRaw; // raw ADC, used for calibration
};

void sensorsBegin();
Reading sensorsRead();
