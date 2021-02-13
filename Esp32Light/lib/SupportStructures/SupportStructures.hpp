#pragma once

enum class EventsType
{
    Show,
    Setup
};

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
    EventsType event_type;
    SetupEvents setup_name;
    ShowEvents show_name;
    LightGroup light_group;
    int event_value;
    CRGB color;
} t_lightEvent;