#pragma once

#include <stdint.h>
#include "FastLED.h"

// ------------------------------------------------------ Pins ------------------------------------------------------ //
#define PIN_LED_STRIP_1 53
#define TOTAL_LEDS_STRIP_1 108
#define PIN_LED_STRIP_2 52
#define TOTAL_LEDS_STRIP_2 120

// ------------------------------------------------------ Leds ------------------------------------------------------ //
#define TOTAL_LEDS (TOTAL_LEDS_STRIP_1 + TOTAL_LEDS_STRIP_2)

// If LEDTYPE is NEOPIXEL it is necessary to edit file main.cpp in line
// FastLED.addLeds<LEDTYPE, DATA_PIN, GRB>(leds, NUM_LEDS);
// with
// FastLED.addLeds<LEDTYPE, DATA_PIN>(leds, NUM_LEDS);
#define LEDTYPE NEOPIXEL

#define POWERLIMIT -1 //max usage of power in milliwatts, -1 for ulimited
#define BAUD_RATE 250000
#define UPDATES_PER_SECOND 90

// --------------------------------------------------- Split parts --------------------------------------------------- //
// A minimum of 7 leds is required
// Splits will be defined as [From - To) meaning from index will be included and To index not
// If first split wil be 0-10, then index of first part will be 0-1-2-3-4-5-6-7-8-9

// If custom splits is not enabled, then splits will all be same length according to TOTAL_LEDS
#define CUSTOM_SPLITS 0

// A full stripe will be divided in 7 parts
// RingLaserLeft - LeftLaser - CenterLightLeft - BackTopLaser - CenterLightRight - RightLaser - RingLaserRight

#define LEDSTART 10
#define LEDSPLIT1 34   // RingLaser # LEDSTART --- LEDSPLIT1
#define LEDSPLIT2 66   // LeftLaser # LEDSPLIT1 --- LEDSPLIT2
#define LEDSPLIT3 98   // Centerlight # etc.
#define LEDSPLIT4 130  // Backtoplight
#define LEDSPLIT5 162  // Centerlight
#define LEDSPLIT6 194  // RightLaser
#define LEDEND 228     // RingLaser

// --------------------------------------------------- End Setup ---------------------------------------------------- //

#define TIME_BETWEEN_UPDATES (int)(1000 / UPDATES_PER_SECOND)

// time between every fade update to led. Also flash.
#define FADE_TIME_MILLIS 20

// When a command to turn on a light comes, minumim of this milliseconds that light will be on
#define MINIUM_TURN_ON_LIGHT_TIME 30

#define BRIGHTNESSDIVIDER 0.4f //Flash events are 60% brighter than normal light on events
#define BRIGHTNESS 255         //If you power your leds by the ESP32 directly, dont use anything higher than 130. 255 is maximum.

// ----------------------------------------------------- Events ----------------------------------------------------- //
// Show events
#define SHOW_EVENTS 1
#define SETUP_EVENTS 2

#define LIGHT_OFF 0

#define RIGHT_COLOR_ON 1
#define RIGHT_COLOR_FLASH 2
#define RIGHT_COLOR_FADE 3

#define LEFT_COLOR_ON 5
#define LEFT_COLOR_FLASH 6
#define LEFT_COLOR_FADE 7

#define LEFTLASERSPEED 12
#define RIGHTLASERSPEED 13

// from 240 there are setup events
#define SETUPEVENTS 251
#define TURNOFFLIGHTS 252

#define LEFTCOLOR 253
#define RIGHTCOLOR 254

#define CHROMAEVENT 255

// Light Groups
#define BACKTOPLASER 0
#define RINGLASER 1
#define LEFTLASER 2
#define RIGHTLASER 3
#define CENTERLIGHT 4

enum class LightGroup
{
    Error,
    BackTopLaser,
    RingLaser,
    LeftLaser,
    RightLaser,
    CenterLight
};

enum class SetupEvents
{
    Error,
    First_Song_Event,
    Turn_Off_Lights,
    Left_Color,
    Right_Color,
    Chroma_Event
};

enum class ShowEvents
{
    Error,
    Light_Off,
    Right_Color_On,
    Right_Color_Flash,
    Right_Color_Fade,
    Left_Color_On,
    Left_Color_Flash,
    Left_Color_Fade,
    Left_Laser_Speed,
    Right_Laser_Speed,
};


// ---------------------------------------------- Various ----------------------------------------------- //

#define byte uint8_t
