#pragma once
#include <stdint.h>

#include "sensors.h"

void displayBegin();
// Re-initialise the LCD after a pump spike may have corrupted it.
void displayRecover();
// pauseMs > 0 shows the soak pause countdown instead of "Pompa: OFF".
void displayShow(const Reading& r, bool pumpOn, uint32_t pauseMs);
