#include <Arduino.h>
#include "Globals.hpp"
#include "Utils.hpp"
#include "SupportStructures.hpp"
#include "LightToSerialParser.hpp"
#include "LightController.hpp"

static CRGB defaultColorLEFT = CRGB(255, 0, 0);
static CRGB defaultColorRIGHT = CRGB(0, 125, 255);

void init_controllers();
void init_lasers();
void controlLight();
void handleEvent();
void applyEventToGroup();