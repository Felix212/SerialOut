#pragma once

#include "Globals.hpp"

typedef struct LightEvent
{
    byte type;
    byte value;
    CRGB color;
} t_lightEvent;

typedef struct StatusLight
{
    byte FADE = 0;
    byte FLASH = 0;
    byte ON = 0;
} t_status;

typedef struct LightController
{
    // in which status is the light
    t_status status;

    // begin led
    int from;
    // to -1 is last led of this light
    int to;

    // color of light
    CRGB color;
    CRGB color_flash;
    CRGB actual_color;
} t_light_controller;

typedef struct CacheColors
{
    CRGB color_left;
    CRGB color_left_flash;
    CRGB color_right;
    CRGB color_right_flash;
} t_cached_colors;

typedef struct Laser
{
    int laserSpeed = 5;
    int laserIndex = 0;
    int strip_part_index;
    bool toggle = true;
} t_laser;
