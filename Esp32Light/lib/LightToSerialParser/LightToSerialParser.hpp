#pragma once

#include "Globals.hpp"
#include "SupportStructures.hpp"

class LightToSerialParser
{

private:
  byte buffer_size = 2;
  byte buffer[2];
  size_t total_read_byte;
  size_t current_pos_buffer;
  void byteToRGB(CRGB *, byte);

public:
  LightToSerialParser();
  void readData();
  bool moreEvents();
  t_lightEvent *parseMessage();
  int dataAvailable();
};
