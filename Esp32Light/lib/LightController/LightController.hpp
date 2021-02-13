#pragma once
#include "Utils.hpp"

class LightController
{
protected:
    CRGBSet *support_array;
    // in which status is the light
    t_status status;

    // begin led index
    int from;
    // to -1 is last led index of this light controller
    int to;

    CRGB color;
    CRGB color_flash;
    CRGB actual_color;
    bool color_changed;

    // time of when light was turn on
    uint32_t status_start_time;
    uint32_t last_fade_time;
    uint32_t current_time;

    // flag to turn of light after minimum time passed
    bool have_to_turn_off;

    void flashLight();
    void fadeLight();
    void update_color(CRGB);
    void update_status(t_status, uint32_t);
    void reset_colors();
    void reset_status();
    void turn_off_leds();
    void compute_actual_color();

public:
    LightController(CRGBSet *, int, int);
    ~LightController();
    void update(uint32_t);
    void handle_event(t_status, CRGB, uint32_t);
    void reset_controller();
};
