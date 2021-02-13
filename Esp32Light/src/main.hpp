#include <Arduino.h>
#include "Globals.hpp"
#include "Utils.hpp"
#include "SupportStructures.hpp"
#include "LightToSerialParser.hpp"
#include "LightController.hpp"
#include "LaserController.hpp"

void init_controllers();
void init_lasers();
void controlLight();
void handleEvent();
void applyEventToGroup();