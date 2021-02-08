#include <Arduino.h>
#include "Globals.hpp"
#include "LedUtils.hpp"
#include "SupportStructures.hpp"
#include "LightToSerialParser.hpp"

static CRGB defaultColorLEFT = CRGB(255, 0, 0);
static CRGB defaultColorRIGHT = CRGB(0, 125, 255);

void init_controllers();
void init_lasers();
void fadeLight();
void fadeFlashLight();
void cacheLeftColor(uint8_t r, uint8_t g, uint8_t b);
void cacheRightColor(uint8_t r, uint8_t g, uint8_t b);
void controlLight();
void ledwalkleft(t_laser *);
void ledwalkright(t_laser *);
