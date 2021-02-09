#include "LightController.hpp"

// -------------------------------------------------- Constructors -------------------------------------------------- //
LightController::LightController(CRGBSet *leds, size_t from, size_t to)
{
    this->support_array = leds;
    this->from = from;
    this->to = to;
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
    this->have_to_turn_off = false;
}

void LightController::update_status(t_status new_status, uint32_t time)
{
    this->status_start_time = time;
    this->last_fade_time = time;

    if (new_status.on)
    {
        this->status = new_status;
        this->have_to_turn_off = false;
        if (new_status.flash)
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
        this->status.fade = false;
        this->status.flash = false;
    }
}

void LightController::flashLight()
{
    this->actual_color = this->actual_color.fadeToBlackBy(1);
    this->color_changed = true;

    if (this->actual_color.getLuma() < this->color.getLuma())
    {
        this->actual_color = this->color;
        this->status.flash = false;
    }
}

void LightController::fadeLight()
{
    this->actual_color = this->actual_color.fadeToBlackBy(1);
    this->color_changed = true;
    if (this->actual_color.getAverageLight() < 16)
    {
        this->actual_color = CRGB::Black;
        reset_status();
    }
}

void LightController::compute_actual_color()
{
    if (this->have_to_turn_off)
    {
        if (this->current_time - this->status_start_time > MINIUM_TURN_ON_LIGHT_TIME)
        {
            this->have_to_turn_off = false;
            this->status.on = false;
            this->actual_color = CRGB::Black;
            this->color_changed = true;
        }
    }
    else if (this->status.fade)
    {
        if(this->current_time - this->last_fade_time > FADE_TIME_MILLIS){
            this->fadeLight();
            this->last_fade_time = this->current_time;
        }
    }
    else if (this->status.flash)
    {
        if(this->current_time - this->last_fade_time > FADE_TIME_MILLIS){
            this->flashLight();
            this->last_fade_time = this->current_time;
        }            
    }
}

void LightController::turn_off_leds()
{
    (*support_array)(this->from, this->to - 1) = CRGB::Black;
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
    this->turn_off_leds();
}

void LightController::update(uint32_t current_millis)
{
    this->current_time = current_millis;

    if (this->status.on)
    {
        this->compute_actual_color();

        // Apply current color to support vector
        if (this->color_changed)
        {
            (*support_array)(this->from, this->to - 1) = this->actual_color;
        }
    }
}