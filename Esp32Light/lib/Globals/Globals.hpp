#pragma once

#include <stdint.h>
#include "FastLED.h"
//SETUP
#define NUM_LEDS 120 //amount of leds

// If LEDTYPE is NEOPIXEL it is necessary to edit file main.cpp in line
// FastLED.addLeds<LEDTYPE, DATA_PIN, GRB>(leds, NUM_LEDS);
// with
// FastLED.addLeds<LEDTYPE, DATA_PIN>(leds, NUM_LEDS);
#define LEDTYPE NEOPIXEL
#define DATA_PIN 13   // GPIO pin
#define POWERLIMIT -1 //max usage of power in milliwatts, -1 for ulimited
#define BAUD_RATE 9600
#define UPDATES_PER_SECOND 30

#define PIN_LED_0 11
#define PIN_LED_1 10
#define PIN_LED_2 9
#define PIN_LED_3 8
#define PIN_LED_4 7

// led strip is split into different sections.
#define LEDSTART 0
#define LEDSPLIT1 8   // RingLaser # LEDSTART --- LEDSPLIT1
#define LEDSPLIT2 25  // LeftLaser # LEDSPLIT1 --- LEDSPLIT2
#define LEDSPLIT3 42  // Centerlight # etc.
#define LEDSPLIT4 59  // Backtoplight
#define LEDSPLIT5 86  // Centerlight
#define LEDSPLIT6 103 // RightLaser
#define LEDEND 120    // RingLaser

//SETUP END

#define DEBUG_MESSAGES 1
#define TIME_BETWEEN_UPDATES (int)(1000 / UPDATES_PER_SECOND)
#define FADE_TIME_MILLIS 20

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

#define byte uint8_t

enum class LightEvents
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

static CRGB defaultColorLEFT = CRGB(255, 0, 0);
static CRGB defaultColorRIGHT = CRGB(0, 125, 255);