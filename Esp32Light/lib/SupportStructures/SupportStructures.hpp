#pragma once

#include "Globals.hpp"

typedef struct StatusLight
{
    int fade;
    int flash;
    int on;
} t_status;

typedef struct CacheColors
{
    CRGB color_left_flash;
    CRGB color_right_flash;
} t_cached_colors;

typedef struct Event
{
    int event_type;
    SetupEvents setup_name;
    ShowEvents show_name;
    LightGroup light_group;
    int event_value;
    CRGB color;
} t_lightEvent;