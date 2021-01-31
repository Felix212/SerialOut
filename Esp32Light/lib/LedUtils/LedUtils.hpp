#pragma once

#include "Globals.hpp"

void checkLeds(CRGB *actual_leds, int from, int to);
void update_leds(CRGB *actual_leds, CRGB *support_array, int from, int to);
void resetModeDisplay();
void turnOnModeDisplay(uint8_t n);