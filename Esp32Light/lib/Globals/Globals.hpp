#pragma once

#include <stdint.h>
#include "FastLED.h"

// -------------------------------------------------- Leds Strips --------------------------------------------------- //
/*
If you need to increase or decrease number of led strips, you also need to update the adding function "addLedsToFastLEDS"
Since you can connect multiple strips, two array of leds will be used. One represents leds updated by light controllers
and one the actual leds connected to board. The function "update_leds" in Utils.hpp copies the support array to the true
leds array. If a certain part of leds has flag "reverse" setted to true, then it will be copied in reverse to actual leds.
For example:
1 2 3 4 5 6 7 8 9 10
1 2 3 4 5 1 2 3 4 5

1 2 3 4 5 6 7 8 9 10
5 4 3 2 1 1 2 3 4 5

In the second case, the first part flag is true.
This is needed since leds strips count leds starting from connection to board, so if you place them one near the other they
have to be reversed.

If you have any questions, contact Kratos#0901 on discord.
*/
#define PIN_LED_STRIP_1 53
#define PIN_LED_STRIP_2 52
#define N_LEDS_STRIPS   {108,  120  }
#define REVERSE_DISPLAY {true, false}

#define TOTAL_STRIPS 2

// Total Leds to use
#define TOTAL_LEDS 228

// ------------------------------------------------------ Leds ------------------------------------------------------ //
// If LEDTYPE is NEOPIXEL it is necessary to edit file Utils.cpp in function addLedsToFastLED at line
// FastLED.addLeds<LEDTYPE, DATA_PIN, GRB>(leds, NUM_LEDS);
// with
// FastLED.addLeds<LEDTYPE, DATA_PIN>(leds, NUM_LEDS);
#define LEDTYPE NEOPIXEL

// Max usage of power in milliwatts through the board. If external power, not from board, is provided to Leds, 
// it can be -1, so unlimited
#define POWERLIMIT -1

// Has to be same that in game
#define BAUD_RATE 250000

// How many times every seconds update Leds
#define UPDATES_PER_SECOND 90

// --------------------------------------------------- Split parts --------------------------------------------------- //
// A minimum of 7 leds is required
// Splits will be defined as [From - To) meaning FROM index will be included and TO index not
// If a split is 4-10, then used indices will be 4-5-6-7-8-9

// If custom splits is not enabled, then splits will all be same length according to TOTAL_LEDS
// Enable: 1 or true
// Disable: 0 or false
#define CUSTOM_SPLITS 0

// A full stripe will be divided in 7 parts
// RingLaserLeft - LeftLaser - CenterLightLeft - BackTopLaser - CenterLightRight - RightLaser - RingLaserRight

#define LEDSTART 10
#define LEDSPLIT1 34  // RingLaser # LEDSTART --- LEDSPLIT1
#define LEDSPLIT2 66  // LeftLaser # LEDSPLIT1 --- LEDSPLIT2
#define LEDSPLIT3 98  // Centerlight # etc.
#define LEDSPLIT4 130 // Backtoplight
#define LEDSPLIT5 162 // Centerlight
#define LEDSPLIT6 194 // RightLaser
#define LEDEND 228    // RingLaser

// --------------------------------------------------- End Setup ---------------------------------------------------- //
// Default Colors
#define DEFAULT_LEFT_COLOR CRGB(255, 0, 0)
#define DEFAULT_RIGHT_COLOR CRGB(0, 125, 255)

// time between every fade update to led. Every update light will be faded to black by 1
#define FADE_TIME_MILLIS 20

// time between every fade update to led. Every update light will be faded to normal color by 1
#define FLASH_TIME_MILLIS 20

// When a command to turn on a light comes, minumim of this milliseconds that light will be on even if a event to turn off comes
#define MINIUM_TURN_ON_LIGHT_TIME 30

// Flash events are 60% brighter than normal light on events
#define BRIGHTNESSDIVIDER 0.4f
// If you power your leds by the ESP32 directly, dont use anything higher than 130. 255 is maximum.
#define BRIGHTNESS_MAX 255

// ----------------------------------------------------- Events ----------------------------------------------------- //
// Show events
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
