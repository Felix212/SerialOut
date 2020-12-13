#pragma once
#include <FastLED.h>


#define NUM_LEDS 150 / 3 //amount of leds. WS
#define LEDTYPE WS2812B
#define DATA_PIN 21 

#define BRIGHTNESS 130 //If you power your leds by the ESP32 directly, dont use anything higher than 130. 255 is maximum.
#define BRIGHTNESSDEVIDER 0.4 //Flash events are 60% brighter than normal light on events


// led strip is split into different sections. Check documention for more info
#define LEDSTART 0 
#define LEDSPLIT1 24 / 3 
#define LEDSPLIT2 48 / 3 
#define LEDSPLIT3 60 / 3 
#define LEDSPLIT4 87 / 3 
#define LEDSPLIT5 99 / 3 
#define LEDSPLIT6 120 / 3 
#define LEDEND 150 / 3 

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