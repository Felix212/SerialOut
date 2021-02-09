#include "LaserController.hpp"

LaserController::LaserController(CRGBSet *leds, size_t from, size_t to, bool direction, size_t speed) : LightController::LightController(leds, from, to)
{
    this->direction = direction;
    this->current_offset = 0;
    set_speed(speed, 0);
}

void LaserController::set_speed(size_t new_speed, uint32_t time)
{
    this->last_laser_time = time;
    if (new_speed == 0)
    {
        this->current_offset = 0;
        this->can_shift = false;
    }
    else
    {
        time_between_laser_updates = 151 / new_speed;
        this->can_shift = true;
    }
}

void LaserController::updateOffset()
{
    // Apply current color to support vector
    // true -> to right
    // 100100 -> 010010
    // false -> to left
    // 100100 -> 001001
    if (direction)
    {
        this->current_offset = (this->current_offset + 1) % 3;
    }
    else
    {
        this->current_offset = this->current_offset - 1;
        if (this->current_offset < 0)
        {
            this->current_offset = 2;
        }
    }
}

void LaserController::colorLeds()
{
    for (int i = this->from; i < this->to; ++i)
    {
        if ((i - this->from) % 3 == this->current_offset)
        {
            (*support_array)[i] = this->actual_color;
        }
        else
        {
            (*support_array)[i] = CRGB::Black;
        }
    }
}

void LaserController::update(uint32_t current_millis)
{
    this->current_time = current_millis;

    if (this->status.on)
    {
        this->compute_actual_color();

        if (this->current_time - this->last_laser_time > this->time_between_laser_updates)
        {
            if (this->can_shift)
            {
                this->updateOffset();
            }
            this->colorLeds();
            this->last_laser_time = this->current_time;
        }
        else if (this->color_changed)
        {
            this->colorLeds();
        }
    }
}
