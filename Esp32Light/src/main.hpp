#pragma once
#include <Arduino.h>
#include <FastLED.h>
#include <BS_LightToSerial.h>

//SETUP
#define NUM_LEDS 120  //amount of leds

// If LEDTYPE is NEOPIXEL it is necessary to edit file main.cpp in line
// FastLED.addLeds<LEDTYPE, DATA_PIN, GRB>(leds, NUM_LEDS);
// with
// FastLED.addLeds<LEDTYPE, DATA_PIN>(leds, NUM_LEDS);
#define LEDTYPE NEOPIXEL 
#define DATA_PIN 13 // GPIO pin
#define POWERLIMIT -1 //max usage of power in milliwatts, -1 for ulimited
#define BAUD_RATE 250000
#define UPDATES_PER_SECOND 90

// led strip is split into different sections.
#define LEDSTART 0 
#define LEDSPLIT1 17 // RingLaser # LEDSTART --- LEDSPLIT1
#define LEDSPLIT2 34 // LeftLaser # LEDSPLIT1 --- LEDSPLIT2
#define LEDSPLIT3 51 // Centerlight # etc.
#define LEDSPLIT4 68 // Backtoplight
#define LEDSPLIT5 85 // Centerlight
#define LEDSPLIT6 102 // RightLaser
#define LEDEND 120 // RingLaser

//SETUP END

#define DEBUG_MESSAGES 0
#define TIME_BETWEEN_UPDATES (int) (1000 / UPDATES_PER_SECOND)

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
#define TURNOFFLIGHTS 252
#define SETUPEVENTS 251

void checkLeds();
void controlLight(struct Lights& l, BS_LightEvent event);
void fadeLight(struct Lights& l);
void fadeFlashLight(struct Lights& l);
void ledwalkleft(struct Laser*, int*, int*);
void ledwalkright(struct Laser*, int*, int*);

struct Status {
  byte FADE = 0;
  byte FLASH = 0;
  byte ON = 0;
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