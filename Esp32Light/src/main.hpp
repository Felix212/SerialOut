#pragma once
#include <FastLED.h>

//SETUP
#define NUM_LEDS 150  //amount of leds
#define LEDTYPE WS2812B 
#define DATA_PIN 21 
#define POWERLIMIT 4500 //in milliwatts 

// led strip is split into different sections.
#define LEDSTART 0 
#define LEDSPLIT1 24 // RingLaser # LEDSTART --- LEDSPLIT1
#define LEDSPLIT2 48 // LeftLaser # LEDSPLIT1 --- LEDSPLIT2
#define LEDSPLIT3 60 // Centerlight # etc.
#define LEDSPLIT4 87 // Backtoplight
#define LEDSPLIT5 99 // Centerlight
#define LEDSPLIT6 120 // RightLaser
#define LEDEND 150 // RingLaser

//SETUP END
#define BRIGHTNESSDEVIDER 0.4 //Flash events are 60% brighter than normal light on events
#define BRIGHTNESS 255 //If you power your leds by the ESP32 directly, dont use anything higher than 130. 255 is maximum.



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
#define RIGHTCOLORON 1
#define RIGHTCOLORFLASH 2
#define RIGHTCOLORFADE 3

#define LEFTCOLORON 5
#define LEFTCOLORFLASH 6
#define LEFTCOLORFADE 7

#define COLORONLY 10

#define CHROMAEVENT 255
#define RIGHTCOLOR 254
#define LEFTCOLOR 253
#define SETUPEVENTS 253

void controlLight(struct Lights& l, BS_LightEvent event);
void fadeLight(struct Lights& l);
void fadeFlashLight(struct Lights& l);
void ledwalkleft(struct Laser*, int*, int*);
void ledwalkright(struct Laser*, int*, int*);

struct Status {
  int FADE = 0;
  int FLASH = 0;
  int ON = 0;
};
struct Lights {
  struct Status status;
  int MIN;
  int MAX;

  CRGB color;
};
struct Laser {
  int laserSpeed = 5;
  int laserIndex = 0;
  int index;
  bool toggle = true;
};