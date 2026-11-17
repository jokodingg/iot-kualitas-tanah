#pragma once
#include "sensors.h"

void displayBegin();
// Re-initialise the LCD after a pump spike may have corrupted it.
void displayRecover();
void displayShow(const Reading& r, bool pumpOn);
