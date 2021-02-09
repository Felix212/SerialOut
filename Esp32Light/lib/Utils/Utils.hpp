#pragma once

#include "Globals.hpp"

void checkLeds(CRGB *actual_leds, int from, int to);
void update_leds(CRGB *actual_leds, CRGB *support_array);
void byteToRGB(CRGB *, byte);
LightGroup valToLightGroup(byte);
SetupEvents nameToSetupEvent(byte);
ShowEvents nameToShowEvent(byte);