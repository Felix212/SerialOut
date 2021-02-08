#pragma once

#include "Globals.hpp"

typedef struct Laser
{
    int laserSpeed = 5;
    int laserIndex = 0;
    int strip_part_index;
    bool toggle = true;
} t_laser;
