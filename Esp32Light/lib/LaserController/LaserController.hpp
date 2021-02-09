#pragma once

#include "Globals.hpp"
#include "LightController.hpp"

class LaserController : public LightController
{
protected:
    size_t speed;
    int current_offset;
    bool direction;
    bool can_shift;
    uint32_t last_laser_time;
    uint32_t time_between_laser_updates;
    void colorLeds();
    void updateOffset();
public:
    LaserController(CRGBSet *, size_t, size_t, bool, size_t);
    ~LaserController();
    void update(uint32_t);
    void set_speed(size_t, uint32_t);
};