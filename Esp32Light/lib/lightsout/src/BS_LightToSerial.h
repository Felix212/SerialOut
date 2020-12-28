// ensure this library description is only included once
#ifndef BS_LightToSerial_h
#define BS_LightToSerial_h

#include <stdint.h>
#define byte uint8_t
struct color
{
  byte r;
  byte g;
  byte b;
};

struct BS_LightEvent
{
  byte type;
  byte value;
  struct color color;
};

// library interface description
class BS_LightToSerial
{
public:
  BS_LightToSerial();

  BS_LightEvent ParseMessage(byte[]);
  color left;
  color right;
  byte bpm;
  enum LightEvents
  {
    Light_Off,
    Right_Color_On,
    Right_Color_Flash,
    Right_Color_Fade,
    Unused,
    Left_Color_On,
    Left_Color_Flash,
    Left_Color_Fade,
  };
};

#endif
