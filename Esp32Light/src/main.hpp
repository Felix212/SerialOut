#pragma once
#include <Arduino.h>
#include <FastLED.h>
#include <BS_LightToSerial.h>

//SETUP
#define NUM_LEDS 150 //amount of leds

// If LEDTYPE is NEOPIXEL it is necessary to edit file main.cpp in line
// FastLED.addLeds<LEDTYPE, DATA_PIN, GRB>(leds, NUM_LEDS);
// with
// FastLED.addLeds<LEDTYPE, DATA_PIN>(leds, NUM_LEDS);
#define LEDTYPE WS2812B
#define DATA_PIN 12   // GPIO pin
#define POWERLIMIT -1 //max usage of power in milliwatts, -1 for ulimited
#define BAUD_RATE 250000
#define UPDATES_PER_SECOND 160

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

#define DEBUG_MESSAGES 0
#define TIME_BETWEEN_UPDATES (int)(1000 / UPDATES_PER_SECOND)

#define BRIGHTNESSDIVIDER 0.4 //Flash events are 60% brighter than normal light on events
#define BRIGHTNESS 255        //If you power your leds by the ESP32 directly, dont use anything higher than 130. 255 is maximum.

//lighttypes
#define BACKTOPLASER 0
#define RINGLASER 1
#define LEFTLASER 2
#define RIGHTLASER 3
#define CENTERLIGHT 4
#define LEFTLASERSPEED 12
#define RIGHTLASERSPEED 13

//lightevents

#define COLORONLY 10

#define CHROMAEVENT 255
#define RIGHTCOLOR 254
#define LEFTCOLOR 253
#define TURNOFFLIGHTS 252
#define SETUPEVENTS 251

void cacheColors(color left, color right);
void checkLeds();
void controlLight(struct Lights &l, BS_LightEvent event);
void fadeLight(struct Lights &l);
void fadeFlashLight(struct Lights &l);
void ledwalkleft(struct Laser *, int *, int *);
void ledwalkright(struct Laser *, int *, int *);

struct Status
{
  byte FADE = 0;
  byte FLASH = 0;
  byte ON = 0;
};

struct Lights
{
  struct Status status;
  int MIN;
  int MAX;

  CRGB color;
  CRGB color_flash;
};

typedef struct CacheColors {
  CRGB color_left;
  CRGB color_left_flash;
  CRGB color_right;
  CRGB color_right_flash;
  struct color left;
  struct color right; 
} t_colors;

struct Laser
{
  int laserSpeed = 5;
  int laserIndex = 0;
  int index;
  bool toggle = true;
};
