#pragma once

#include "Globals.hpp"

typedef struct StatusLight
{
    bool fade;
    bool flash;
    bool on;
} t_status;

typedef struct CacheColors
{
    CRGB left;
    CRGB right;
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