// ensure this library description is only included once
#pragma once
#include "SupportStructures.hpp"

// library interface description
// Color byte is decoded as follows
// [ r r r g g g b b ]

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
