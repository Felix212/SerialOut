#pragma once

#include "Globals.hpp"

void checkLeds(CRGB *actual_leds, int from, int to);
void update_leds(CRGB *actual_leds, CRGB *support_array);