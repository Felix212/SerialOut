#pragma once
#include "LightController.hpp"

class LaserController : public LightController
{
protected:
    int speed;
    int current_offset;
    bool direction;
    bool can_shift;
    uint32_t last_laser_time;
    uint32_t time_between_laser_updates;
    void colorLeds();
    void updateOffset();

public:
    LaserController(CRGBSet *, int, int, bool, int);
    ~LaserController();

    // current time
    void update(uint32_t);

    // New status, Color to apply, current time
    void handle_event(t_status, CRGB, uint32_t);

    void reset_controller();
    void set_speed(int, uint32_t);
};