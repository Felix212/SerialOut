#pragma once

#include "Globals.hpp"
#include "SupportStructures.hpp"

class LightToSerialParser
{

private:
  byte buffer_size = 2;
  byte buffer[2];
  // first part of event to handle
  byte ev_0;
  // second part of event to handle
  byte ev_1;
  void byteToRGB(CRGB *, byte);
  SerialEvents nameToLightEvent(byte);
  LightGroup valToLightGroup(byte);
public:
  LightToSerialParser();
  void readData();
  bool moreEvents();
  t_lightEvent * parseMessage();
};
