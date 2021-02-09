#include "LightController.hpp"

// -------------------------------------------------- Constructors -------------------------------------------------- //
LightController::LightController(CRGBSet *leds, size_t from, size_t to)
{
    this->support_array = leds;
    this->from = from;
    this->to = to;
    this->color_changed = true;
    this->reset_colors();
    this->reset_status();
}

LightController::~LightController()
{
}

// ------------------------------------------------ Private Methods ------------------------------------------------- //
void LightController::update_color(CRGB flash_color)
{
    this->color_changed = true;

    this->color_flash.setRGB(flash_color.r,
                                flash_color.g,
                                flash_color.b);
    this->color.setRGB((uint8_t)((float)flash_color.r * BRIGHTNESSDIVIDER),
                        (uint8_t)((float)flash_color.g * BRIGHTNESSDIVIDER),
                        (uint8_t)((float)flash_color.b * BRIGHTNESSDIVIDER));
}

void LightController::reset_colors()
{
    this->color_changed = true;
    this->color = CRGB::Black;
    this->color_flash = CRGB::Black;
    this->actual_color = CRGB::Black;
}

void LightController::reset_status()
{
    this->status_start_time = 0;
    this->status.fade = 0;
    this->status.flash = 0;
    this->status.on = 0;
}

void LightController::update_status(t_status new_status, uint32_t time)
{
    this->status = new_status;
    this->status_start_time = time;

    if (this->status.on == 1)
    {
        if (this->status.flash == 1)
        {
            this->actual_color = this->color_flash;
        }
        else
        {
            this->actual_color = this->color;
        }
    }
    else
    {
        this->have_to_turn_off = true;
    }
}

void LightController::flashLight()
{
    this->actual_color = this->actual_color.fadeToBlackBy(1);

    if (this->actual_color.getLuma() > this->color.getLuma())
    {
        (*support_array)(this->from, this->to - 1) = this->actual_color;
    }
    else
    {
        (*support_array)(this->from, this->to - 1) = this->color;
        this->actual_color = this->color;
        this->status.flash = 0;
    }
}

void LightController::fadeLight()
{
    this->actual_color = this->actual_color.fadeToBlackBy(1);

    if (this->actual_color.getAverageLight() > 16)
    {
        (*support_array)(this->from, this->to - 1) = this->actual_color;
    }
    else
    {
        (*support_array)(this->from, this->to - 1) = CRGB::Black;
        this->actual_color = CRGB::Black;
        reset_status();
    }
}

// ------------------------------------------------- Public Methods ------------------------------------------------- //
void LightController::handle_event(t_status new_status, CRGB new_color, uint32_t event_time)
{
    // If color is not black
    if (new_color.getAverageLight() > 0)
    {
        this->update_color(new_color);
    }
    this->update_status(new_status, event_time);
}

void LightController::reset_controller()
{
    this->reset_status();
    this->reset_colors();
}

void LightController::update(uint32_t current_millis)
{
    if (this->have_to_turn_off)
    {
        if (current_millis - this->status_start_time > MINIUM_TURN_ON_LIGHT_TIME)
        {
            this->have_to_turn_off = false;
            (*support_array)(this->from, this->to - 1) = CRGB::Black;
            this->actual_color = CRGB::Black;
        }
    }
}