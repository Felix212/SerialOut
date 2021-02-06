#include "LedUtils.hpp"

// Checks leds by turning all them on with colors RED -> GREEN -> BLUE
void checkLeds(CRGB *actual_leds, int from, int to)
{
    //Led check
    for (int i = from; i < to; i++)
    {
        actual_leds[i] = CRGB::Black;
    }

    FastLED.show();
    delay(500);

    for (int i = from; i < to; i++)
    {
        actual_leds[i] = CRGB::Red;
    }

    FastLED.show();
    delay(500);

    for (int i = from; i < to; i++)
    {
        actual_leds[i] = CRGB::Green;
    }

    FastLED.show();
    delay(500);

    for (int i = from; i < to; i++)
    {
        actual_leds[i] = CRGB::Blue;
    }

    FastLED.show();
    delay(500);

    for (int i = from; i < to; i++)
    {
        actual_leds[i] = CRGB::Black;
    }
    
    FastLED.show();
}
