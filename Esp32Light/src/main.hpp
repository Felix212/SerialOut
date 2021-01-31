#include <Arduino.h>
#include "Globals.hpp"
#include "LightToSerialParser.hpp"
#include "LedUtils.hpp"

void init_controllers();
void fadeLight(int);
void fadeFlashLight(int);
void cacheColors(CRGB *, CRGB *);
void handleChromaEvent(byte);
void handleEvent();
void controlLight(int, t_lightEvent *);
void ledwalkleft(t_laser *, int, int);
void ledwalkright(t_laser *, int, int);