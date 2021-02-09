#include "Utils.hpp"

// Checks leds by turning all them on with colors RED -> GREEN -> BLUE
void checkLeds(CRGB *actual_leds, int from, int to)
{
    //Led check
    for (int i = from; i < to; ++i)
    {
        actual_leds[i] = CRGB::Black;
    }

    FastLED.show();
    delay(500);

    for (int i = from; i < to; ++i)
    {
        actual_leds[i] = CRGB::Red;
    }

    FastLED.show();
    delay(500);

    for (int i = from; i < to; ++i)
    {
        actual_leds[i] = CRGB::Green;
    }

    FastLED.show();
    delay(500);

    for (int i = from; i < to; ++i)
    {
        actual_leds[i] = CRGB::Blue;
    }

    FastLED.show();
    delay(500);

    for (int i = from; i < to; ++i)
    {
        actual_leds[i] = CRGB::Black;
    }
    
    FastLED.show();
}

void update_leds(CRGB *actual_leds, CRGB *support_array)
{
    int i, k = 0;
    for (i = TOTAL_LEDS_STRIP_1 - 1; i >= 0; --i)
    {
      actual_leds[i] = support_array[k];
      k = k + 1;
    }
    for (i = TOTAL_LEDS_STRIP_1; i < TOTAL_LEDS; ++i)
    {
      actual_leds[i] = support_array[k];
      k = k + 1;
    }
}

void byteToRGB(CRGB *color, byte data_color)
{
	color->r = (data_color >> 5) * 32;
	color->g = ((data_color & 28) >> 2) * 32;
	color->b = (data_color & 3) * 64;
}

LightGroup valToLightGroup(byte val)
{
	switch (val)
	{
	case BACKTOPLASER:
		return LightGroup::BackTopLaser;
	case RINGLASER:
		return LightGroup::RingLaser;
	case LEFTLASER:
		return LightGroup::LeftLaser;
	case RIGHTLASER:
		return LightGroup::RightLaser;
	case CENTERLIGHT:
		return LightGroup::CenterLight;
	default:
		return LightGroup::Error;
	}
}

SetupEvents nameToSetupEvent(byte name)
{
	switch (name)
	{
	case SETUPEVENTS:
		return SetupEvents::First_Song_Event;
	case TURNOFFLIGHTS:
		return SetupEvents::Turn_Off_Lights;
	case LEFTCOLOR:
		return SetupEvents::Left_Color;
	case RIGHTCOLOR:
		return SetupEvents::Right_Color;
	case CHROMAEVENT:
		return SetupEvents::Chroma_Event;
	default:
		return SetupEvents::Error;
	}
}

ShowEvents nameToShowEvent(byte name)
{
	switch (name)
	{
	case LIGHT_OFF:
		return ShowEvents::Light_Off;
	case RIGHT_COLOR_ON:
		return ShowEvents::Right_Color_On;
	case RIGHT_COLOR_FADE:
		return ShowEvents::Right_Color_Fade;
	case RIGHT_COLOR_FLASH:
		return ShowEvents::Right_Color_Flash;
	case LEFT_COLOR_ON:
		return ShowEvents::Left_Color_On;
	case LEFT_COLOR_FADE:
		return ShowEvents::Left_Color_Fade;
	case LEFT_COLOR_FLASH:
		return ShowEvents::Left_Color_Flash;
	case LEFTLASERSPEED:
		return ShowEvents::Left_Laser_Speed;
	case RIGHTLASERSPEED:
		return ShowEvents::Right_Laser_Speed;
	default:
		return ShowEvents::Error;
	}
}