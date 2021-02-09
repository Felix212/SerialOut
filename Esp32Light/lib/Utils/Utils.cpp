#include "Utils.hpp"

void calculate_led_splits(size_t *parts)
{
    if (CUSTOM_SPLITS)
    {
        parts[0] = LEDSTART;
        parts[1] = LEDSPLIT1;
        parts[2] = LEDSPLIT2;
        parts[3] = LEDSPLIT3;
        parts[4] = LEDSPLIT4;
        parts[5] = LEDSPLIT5;
        parts[6] = LEDSPLIT6;
        parts[7] = LEDEND;
    }
    else
    {
        int increment = TOTAL_LEDS / 7;
        int missing_leds = TOTAL_LEDS - increment * 7;

        for (size_t i = 1; i < 8; ++i)
        {
            parts[i] = parts[i - 1] + increment;
        }

        switch (missing_leds)
        {
        // BackTopLaser
        case 1:
            parts[1] += 0;
            parts[2] += 0;
            parts[3] += 0;
            parts[4] += 1;
            parts[5] += 1;
            parts[6] += 1;
            parts[7] += 1;
            break;
        // Both Lasers
        case 2:
            parts[1] += 0;
            parts[2] += 1;
            parts[3] += 1;
            parts[4] += 1;
            parts[5] += 1;
            parts[6] += 2;
            parts[7] += 2;
            break;
        // Both Lasers - BackTopLaser
        case 3:
            parts[1] += 0;
            parts[2] += 1;
            parts[3] += 1;
            parts[4] += 2;
            parts[5] += 2;
            parts[6] += 3;
            parts[7] += 3;
            break;
        case 4:
            // Both Lasers - Both Center Lights
            parts[1] += 0;
            parts[2] += 1;
            parts[3] += 2;
            parts[4] += 2;
            parts[5] += 3;
            parts[6] += 4;
            parts[7] += 4;
            break;
        // Both Lasers - Both Center Lights - BackTopLaser
        case 5:
            parts[1] += 0;
            parts[2] += 1;
            parts[3] += 2;
            parts[4] += 3;
            parts[5] += 4;
            parts[6] += 5;
            parts[7] += 5;
            break;
        // Both Lasers - Both Center Lights - Both RingLights
        case 6:
            parts[1] += 1;
            parts[2] += 2;
            parts[3] += 3;
            parts[4] += 3;
            parts[5] += 4;
            parts[6] += 5;
            parts[7] += 6;
            break;

        default:
            break;
        }
    }
}

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