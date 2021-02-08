#include "LedUtils.hpp"

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