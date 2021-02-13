#pragma once

#include "Globals.hpp"
#include "SupportStructures.hpp"

void addLedsToFastLEDS(CRGB *);
void calculate_led_splits(size_t *parts);
void checkLeds(CRGB *actual_leds, int from, int to);
void update_leds(CRGB *, CRGB *, int *, bool *);
void byteToRGB(CRGB *, byte);

// Checks if two colors are same
bool sameColor(CRGB *, CRGB *);

LightGroup valToLightGroup(byte);
SetupEvents nameToSetupEvent(byte);
ShowEvents nameToShowEvent(byte);
