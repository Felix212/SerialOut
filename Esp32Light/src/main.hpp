#pragma once
#include <FastLED.h>


#define NUM_LEDS 50

#define DATA_PIN 21

#define BRIGHTNESS 255
#define BRIGHTNESSDEVIDER 0.4

#define L1_MIN 0
#define L2_MIN 8
#define L3_MIN 16
#define L4_MIN 20
#define L5_MIN 29
#define L6_MIN 33
#define L7_MIN 40




#define L1_MAX L2_MIN
#define L2_MAX L3_MIN
#define L3_MAX L4_MIN
#define L4_MAX L5_MIN
#define L5_MAX L6_MIN
#define L6_MAX L7_MIN
#define L7_MAX 50


//lighttypes
#define BACKTOPLASER 0
#define RINGLASER 1
#define LEFTLASER 2
#define RIGHTLASER 3
#define CENTERLIGHT 4
#define LEFTLASERSPEED 12
#define RIGHTLASERSPEED 13

//lightevents

#define LIGHTOFF 0
#define LEFTCOLORON 1
#define LEFTCOLORFADE 2
#define LEFTCOLORFLASH 3
#define RIGHTCOLORON 5
#define RIGHTCOLORFADE 6
#define RIGHTCOLORFLASH 7
#define COLORONLY 10

void controlLight(struct Lights& l, BS_LightEvent event);
void fadeLight(struct Lights l, int);
void fadeFlashLight(struct Lights l, int);
void ledwalkleft(struct Laser*, int*, int*);
void ledwalkright(struct Laser*, int*, int*);


struct Lights {
  int FADE = 0;
  int FLASH = 0;
  int MIN;
  int MAX;
  int ON = 0;
  CRGB color;
};
struct Laser {
  int laserSpeed = 5;
  int laserIndex = 0;
  int index;
  bool toggle = true;
};